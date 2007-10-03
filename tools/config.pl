#!/usr/bin/perl
#
# Name:		config.pl
# Description:	Generate the makefiles and configuration files required for
#		building the program
#

use strict;
use IO::File;

my @srcdirs = ("src");

##
# This variable can be set anywhere and will cause the program to bail out
# at the next most convenient spot.
my $fatal_error = 0;

##
# These lists are used to collect the final processed set of data from
# which the ouputs are created.
my $decls = { };
my $adds = { 'table' => { } };
my $binds = { };
my $options = { };
my $enables = { };
my $modules = { };
my $patterns = { };
my $emits = [ ];

#########
# Start #
#########

main();
exit(0);

sub main {
	## Parse the file
	my $tree = parse_file($ARGV[0]);
	return(0) unless ($tree);

	## Convert all names to full names
	return(0) if (convert_names($tree, ""));

	## Process all variable assignments to find the final value for each variable
	return(0) if (process_configuration($tree));

	# TODO do a dependancy check
	# TODO implement a way to specify that a part of a pattern should be verified
	# TODO implement a verification that all commands/types/variables/etc are declared
	## Check all variable declarations to ensure their type is in the types list
	#return(0) if (check_types($decls, $types));

	## Build and write the config.h file to disc
	build_config_h("config.h");
	return(0);
}

###########################
# Name Conversion Section #
###########################

sub convert_names {
	my ($tree, $namespace) = @_;

	# TODO do option name substitutions in the body of the option
	foreach my $element (@{ $tree }) {
		if ($element->{'type'} eq "namespace") {
			if ($element->{'name'} !~ /^\w(\w|\.)*?\w+$/) {
				print "Error: $element->{'info'}: Invalid namespace name \"$element->{'name'}\"\n";
				$fatal_error = 1;
			}
			else {
				$namespace = ($element->{'name'} eq "null") ? "" : $element->{'name'};
			}
		}
		elsif (($element->{'type'} eq "option")
		    or ($element->{'type'} eq "declaration")
		    or ($element->{'type'} eq "require")
		    or ($element->{'type'} eq "enable")
		    or ($element->{'type'} eq "code")
		    or ($element->{'type'} eq "set")
		    or (($element->{'type'} eq "add") and ($element->{'specifier'} ne "type"))) {
			if ($element->{'name'} !~ /^(\w|\.)+$/) {
				print "Error: $element->{'info'}: Invalid name \"$element->{'name'}\"\n";
				$fatal_error = 1;
			}
			else {
				if ($element->{'name'} =~ /^\./) {
					$element->{'name'} =~ s/^\.//;
				}
				elsif ($namespace) {
					$element->{'name'} = "$namespace.$element->{'name'}";
				}
				convert_names($element->{'block'}, $namespace) if ($element->{'type'} eq "option");
			}
		}
		elsif ($element->{'type'} eq "bind") {
			if ($element->{'command'} !~ /^(\w|\.)+$/) {
				print "Error: $element->{'info'}: Invalid name \"$element->{'command'}\"\n";
				$fatal_error = 1;
			}
			elsif ($element->{'command'} =~ /^\./) {
				$element->{'command'} =~ s/^\.//;
			}
			elsif ($namespace) {
				$element->{'command'} = "$namespace.$element->{'command'}";
			}
		}
	}
	return(-1) if ($fatal_error);
	return(0);
}

#######################
# Interpreter Section #
#######################

sub process_configuration {
	my ($tree) = @_;

	foreach my $element (@{ $tree }) {
		if ($element->{'type'} eq "emit") {
			push(@{ $emits }, $element->{'code'});
		}
		elsif ($element->{'type'} eq "option") {
			my $name = $element->{'name'};
			$options->{ $name } = [ ] unless (defined($options->{ $name }));
			push(@{ $options->{ $name } }, $element);
		}
		elsif ($element->{'type'} eq "register") {
			my $name = $element->{'name'};
			if (defined($patterns->{ $name })) {
				print "Error: $element->{'info'}: Pattern \"$name\" is already declared.\n";
				$fatal_error = 1;
			}
			else {
				$patterns->{ $name } = $element;
			}
		}
		elsif ($element->{'type'} eq "require") {

		}
		elsif ($element->{'type'} eq "enable") {
			my $name = $element->{'name'};
			if (!defined($options->{ $name })) {
				print "Error: $element->{'info'}: Option \"$name\" not declared.\n";
				$fatal_error = 1;
			}
			else {
				# TODO take the specifier into account
				$enables->{ $name } = $element;
				foreach my $option (@{ $options->{ $name } }) {
					process_configuration($option->{'block'});
				}
			}
		}
		elsif ($element->{'type'} eq "module") {
			$modules->{ $element->{'name'} } = $element;
		}
		elsif ($element->{'type'} eq "add") {
			my ($specifier, $name) = ($element->{'specifier'}, $element->{'name'});
			$adds->{ $specifier } = { } unless (defined($adds->{ $specifier }));
			# TODO implement the "duplicate" argument (in $patterns)
			if (!defined($patterns->{ $specifier })) {
				print "Error: $element->{'info'}: Specifier \"$specifier\" is not registered.\n";
				$fatal_error = 1;
			}
			elsif (defined($adds->{ $specifier }->{ $name })) {
				print "Error: $element->{'info'}: " . ucfirst($specifier) . " \"$name\" is already declared.\n";
				$fatal_error = 1;
			}
			else {
				$adds->{ $specifier }->{ $name } = $element;
			}
		}
		elsif ($element->{'type'} eq "bind") {
			$binds->{ $element->{'name'} } = $element;
		}
		elsif (($element->{'type'} eq "declaration") or ($element->{'type'} eq "code")) {
			# TODO the order that types are initialized is important (attrib:fe)
			my $name = $element->{'name'};
			if (defined($decls->{ $name })) {
				print "Error: $element->{'info'}: Duplicate declaration: \"$name\"\n";
				$fatal_error = 1;
			}
			else {
				add_tables($name, $adds);
				$decls->{ $name } = { %{ $element } };
			}
		}
		elsif ($element->{'type'} eq "set") {
			my $name = $element->{'name'};
			if (!defined($decls->{ $name })) {
				print "Error: $element->{'info'}: Variable \"$name\" not declared\n";
				$fatal_error = 1;
			}
			else {
				$decls->{ $name }->{'value'} = $element->{'value'};
			}
		}
	}
	return(-1) if ($fatal_error);
	return(0);
}

sub add_tables {
	my ($name, $adds) = @_;

	my $table = $name;
	while (1) {
		return unless ($table =~ /^(.*)\.(.*?)/);
		$table = $1;
		return if (!$table or defined($adds->{ 'table' }->{ $table }));
		$adds->{ 'table' }->{ $table } = {
			'info' => $name,
			'type' => "add",
			'specifier' => "table",
			'name' => $table,
			'values' => [ ]
		};
	}
}

########################
# Type Checker Section #
########################

=not used atm
sub check_types {
	my ($decls, $types) = @_;

	foreach my $name (keys(%{ $decls })) {
		my $type = $decls->{ $name }->{'type'}; 
		if ($type eq "table") {
			$fatal_error = 1 if (check_types($decls->{ $name }->{'decls'}, $types));
		}
		elsif (($type ne "code") and !defined($types->{ $type })) {
			print "Error: The type \"$type\" of variable \"$name\" is not declared.\n";
			$fatal_error = 1;
		}
	}
	return(-1) if ($fatal_error);
	return(0);
}
=cut

##################
# Output Section #
##################

sub build_config_h {
	my ($file) = @_;

	my ($vars, $code) = sort_by_type($decls);
	(my $fd = new IO::File("$file", "w")) or (print "Unable to open $file for writing\n" and return);
	# TODO print file header
	print $fd "/*\n";
	print $fd " * Automatically generated configuration\n";
	print $fd " */\n\n";
	print $fd "#ifndef _CONFIG_H\n";
	print $fd "#define _CONFIG_H\n\n";

	print $fd "/** Emits */\n";
	foreach my $line (@{ $emits }) {
		print $fd "$line\n";
	}
	print $fd "\n\n";

	print $fd "/** Modules */\n";
	my $includes = "";
	my $init_macro = "#define INIT_MODULES()\t\\\n";
	my $release_macro = "#define RELEASE_MODULES()\t\\\n";
	foreach my $key (sort(keys(%{ $modules }))) {
		my $name = lc($key);
		$includes .= "#include <stutter/modules/$name.h>\n";
		$init_macro .= "\tinit_$name();\t\\\n";
		$release_macro .= "\trelease_$name();\t\\\n";
	}
	print $fd "$includes\n";
	print $fd "$init_macro\n";
	print $fd "$release_macro\n";
	print $fd "\n";

	print $fd "/** Configuration Options */\n";
	foreach my $key (sort(keys(%{ $enables }))) {
		my $name = $key;
		$name =~ tr/a-zA-Z0-9_/_/cs;
		$name = uc($name);
		print $fd "#define CONFIG_$name\n";
	}
	print $fd "\n\n";

	print $fd "/** Definitions */\n";
	foreach my $key (sort(keys(%{ $code }))) {
		my $value = $code->{ $key }->{'value'};
		my $name = uc($key);
		$name =~ tr/a-zA-Z0-9_/_/cs;
		print $fd align_string("#define $name", 48) . "$value\n";
	}
	print $fd "\n\n";

	foreach my $specifier (sort(keys(%{ $patterns }))) {
		next if ($patterns->{ $specifier }->{'args'} =~ /v/);
		my $header = $patterns->{ $specifier }->{'header'};
		$header = "#define ADD_%NS()" unless ($header);
		print $fd format_add_header($specifier, $header) . "\t\\\n";
		if (defined($adds->{ $specifier })) {
			foreach my $key (sort(keys(%{ $adds->{ $specifier } }))) {
				print $fd "\t" . format_add_output($adds->{ $specifier }->{ $key }, $specifier) . "\t\\\n";
			}
		}
		print $fd "$patterns->{ $specifier }->{'footer'}\t\\\n" if ($patterns->{ $specifier }->{'footer'});
		print $fd "\n\n";
	}

	print $fd "/** Key Bindings */\n";
	print $fd "#define ADD_BINDINGS()\t\\\n";
	foreach my $key (sort(keys(%{ $binds }))) {
		my ($name, $cmd, $value) = ($binds->{ $key }->{'name'}, $binds->{ $key }->{'command'}, $binds->{ $key }->{'value'});
		$name = format_value($name);
		$cmd = format_value($cmd);
		$value = $value ? format_value($value) : "NULL";
		print $fd "\tBIND_KEY($name, $cmd, $value)\t\\\n";
	}
	print $fd "\n\n";

	print $fd "/** Variables */\n";
	print $fd "#define ADD_VARIABLES()\t\\\n";
	foreach my $specifier (sort(keys(%{ $patterns }))) {
		next if ($patterns->{ $specifier }->{'args'} !~ /v/);
		my $header = $patterns->{ $specifier }->{'header'};
		$header = "DECLARE_TYPE(\"%n\"," unless ($header);
		print $fd "\t" . format_add_header($specifier, $header) . "\t\\\n";
		if (defined($adds->{ $specifier })) {
			foreach my $key (sort(keys(%{ $adds->{ $specifier } }))) {
				print $fd "\t\t" . format_add_output($adds->{ $specifier }->{ $key }, $specifier) . "\t\\\n";
			}
		}
		if ($patterns->{ $specifier }->{'footer'}) {
			print $fd "\t$patterns->{ $specifier }->{'footer'}\t\\\n";
		}
		else {
			print $fd "\t)\t\\\n";
		}
	}
	foreach my $vartype (sort(keys(%{ $vars }))) {
		my $typename = format_value($vartype);
		print $fd "\tDECLARE_TYPE($typename,\t\\\n";
		foreach my $key (sort(keys(%{ $vars->{ $vartype } }))) {
			my $name = format_value($key);
			my $value = format_value($vars->{ $vartype }->{ $key }->{'value'});
			print $fd "\t\tADD_VARIABLE($name, \"string\", $value)\t\\\n";
		}
		print $fd "\t)\t\\\n";
	}
	print $fd "\n\n";

	print $fd "#endif\n\n";
	close($fd);
}

sub format_add_output {
	my ($add, $specifier) = @_;

	my ($name, $value, @values) = ($add->{'name'}, $add->{'value'}, @{ $add->{'values'} });
	my $str = $patterns->{ $specifier }->{'pattern'};
	if (!defined($str)) {
		print "Error $add->{'info'}: The specifier \"$specifier\" is not registered.\n";
		$fatal_error = 1;
	}
	else {
		for my $i (1..5) {
			$str =~ s/\%p$i/ $values[$i - 1] ? $values[$i - 1] : "NULL" /eg;
			$str =~ s/\%s$i/ $values[$i - 1] ? format_value($values[$i - 1]) : "NULL" /eg;
		}
		$str =~ s/\%n/$name/g;
		$str =~ s/\%s/\"$name\"/g;
		$str =~ s/\%a/$value/g;
	}
	return($str);
}

sub format_add_header {
	my ($name, $pattern) = @_;

	my $NAME = uc($name);
	$NAME =~ tr/a-zA-Z0-9_/_/cs;
	my $str = $pattern;
	$str =~ s/\%n/$name/g;
	$str =~ s/\%N/$NAME/g;
	return($str);
}

sub sort_by_type {
	my ($decls) = @_;

	my ($vars, $code) = ({ }, { });
	foreach my $name (keys(%{ $decls })) {
		my $vartype = $decls->{ $name }->{'vartype'};
		if ($vartype) {
			$vars->{ $vartype } = { } unless (defined($vars->{ $vartype }));
			$vars->{ $vartype }->{ $name } = $decls->{ $name };
		}
		else {
			$code->{ $name } = $decls->{ $name };
		}
	}
	return($vars, $code);
}

##################
# Parser Section #
##################

sub parse_file {
	my ($file) = @_;

	$file =~ /(.*)(\/|\\)(.*?)/;
	my $dir = $1;
	my ($linenum, $thisline) = (1, 1);

	my $tree = [ ];
	(my $fd = new IO::File("$file", "r")) or (print "Error: Unable to open $file\n" and return);
	while (defined(my $line = read_line($fd, \$linenum))) {
		if (!$line) {
			## Ignore blank lines
		}
		elsif ($line =~ /^\s*include\s+(.*?)\s*$/i) {
			# TODO you need to fix namespaces here. I have a feeling the namespace set in an
			#	include will carry on to the host file
			my $filename = (($1 =~ /^\//) or !$dir) ? $1 : "$dir/$1";
			parse_include_file($filename, $tree, "$file:$thisline");
		}
		elsif ($line =~ /^\s*option\s+(\S+?)(|\s+(.+?))\s*$/i) {
			my ($name, $cmd) = (lc($1), $3);
			my $option = {
				'info' => "$file:$thisline",
				'type' => "option",
				'name' => $name,
				'block' => [ ]
			};
			if ($cmd) {
				## Substitute the option name into the command
				$cmd =~ s/\%/$name/g;
				if (($cmd !~ /^\s*end\s*$/) and parse_statement($cmd, $option->{'block'}, "$file:$thisline")) {
					$fatal_error = 1;
					print "Error $file:$thisline: Parse error at \"$cmd\"\n";
				}
			}
			else {
				# TODO do substitution inside the block
				push(@{ $option->{'block'} }, @{ parse_block($fd, $file, $dir, \$linenum) });
			}
			push(@{ $tree }, $option);
		}
		elsif (parse_statement($line, $tree, "$file:$thisline")) {
			$fatal_error = 1;
			print "Error $file:$thisline: Parse error at \"$line\"\n";
		}
		$thisline = $linenum;
	}
	close($fd);
	return(undef) if ($fatal_error);
	return($tree);
}

sub parse_block {
	my ($fd, $file, $dir, $linenum) = @_;

	my $thisline = $$linenum;

	my $tree = [ ];
	while (defined(my $line = read_line($fd, $linenum))) {
		if (!$line) {
			## Ignore blank lines
		}
		elsif ($line =~ /^\s*end\s*$/) {
			return($tree);
		}
		elsif (parse_statement($line, $tree, "$file:$thisline")) {
			$fatal_error = 1;
			print "Error $file:$thisline: Parse error at \"$line\"\n";
		}
		$thisline = $$linenum;
	}
	return($tree);
}

sub parse_statement {
	my ($line, $tree, $info) = @_;

	if ($line =~ /^\s*emit\s+(.+?)\s*$/i) {
		my ($code) = ($1);
		push(@{ $tree }, {
			'info' => $info,
			'type' => "emit",
			'code' => $code
		});
	}
	elsif ($line =~ /^\s*namespace\s+(.+?)\s*$/i) {
		my ($name) = (lc($1));
		push(@{ $tree }, {
			'info' => $info,
			'type' => "namespace",
			'name' => $name
		});
	}
	elsif ($line =~ /^\s*register(?:\s+-([dv]+)|)\s+(\S+?)\s+(?:\[\s*(.*?)\s*\]\s*|)(.*?)\s*(?:\[\s*(.*?)\s*\]\s*|)$/i) {
		my ($args, $name, $header, $pattern, $footer) = ($1, lc($2), $3, $4, $5);
		push(@{ $tree }, {
			'info' => $info,
			'type' => "register",
			'name' => $name,
			'args' => $args,
			'header' => $header,
			'footer' => $footer,
			'pattern' => $pattern
		});
	}
	elsif ($line =~ /^\s*(enable|require)\s+(?:(\S+):|)(\S+?)\s*$/i) {
		my ($keyword, $specifier, $name) = (lc($1), lc($2), lc($3));
		push(@{ $tree }, {
			'info' => $info,
			'type' => $keyword,
			'specifier' => $specifier,
			'name' => $name
		});
	}
	elsif ($line =~ /^\s*(module|frontend)\s+(\S+?)\s*$/i) {
		my ($keyword, $name) = (lc($1), lc($2));
		push(@{ $tree }, {
			'info' => $info,
			'type' => $keyword,
			'name' => $name
		});
		my $dir = get_module_dir($name, $keyword);
		parse_include_file("$dir/defaults.cfg", $tree, $info) if ($dir);
	}
	elsif ($line =~ /^\s*add\s+(\S+?)\s+(\S+?)\s+(.*?)\s*$/i) {
		my ($specifier, $name, $value) = (lc($1), lc($2), $3);
		push(@{ $tree }, {
			'info' => $info,
			'type' => "add",
			'specifier' => $specifier,
			'name' => $name,
			'value' => $value,
			'values' => [ parse_values($value) ]
		});
	}
	elsif ($line =~ /^\s*bind\s+(\S+?)\s+(\S+?)(?:\s+(.*?)|)\s*$/i) {
		# TODO add the context option
		my ($name, $cmd, $value) = (lc($1), $2, $3);
		push(@{ $tree }, {
			'info' => $info,
			'type' => "bind",
			'name' => $name,
			'command' => $cmd,
			'value' => $value
		});
	}
	elsif ($line =~ /^\s*code\s+(\S+?)\s+(.+?)\s*$/i) {
		my ($name, $value) = (lc($1), $2);
		push(@{ $tree }, {
			'info' => $info,
			'type' => "code",
			'name' => $name,
			'value' => $value
		});
	}
	elsif ($line =~ /^\s*set\s+(?:-(\S+?)\s+|)(\S+?)\s+(.+?)\s*$/i) {
		my ($vartype, $name, $value) = (lc($1), lc($2), $3);
		my $type = $vartype ? "declaration" : "set";
		push(@{ $tree }, {
			'info' => $info,
			'type' => $type,
			'name' => $name,
			'vartype' => $vartype,
			'value' => $value
		});
	}
	else {
		return(-1);
	}
	return(0);
}

sub parse_values {
	my ($values) = @_;

	my @ret = ();
	while ($values) {
		my $value;
		if ($values =~ /\"([^"]+)\"/) {
			$value = $1;
			$values =~ s/\"([^"]+)\"(\s+|)//;
		}
		elsif ($values =~ /([^\s"]+)/) {
			$value = $1;
			$values =~ s/([^\s"]+)(\s+|)//;
		}
		else {
			last;
		}
		push(@ret, $value);
	}
	return(@ret);
}

sub parse_include_file {
	my ($filename, $tree, $info) = @_;

	my $filetree = parse_file($filename);
	if (!defined($filetree)) {
		$fatal_error = 1;
		print "Error $info: Error parsing include file \"$filename\"\n";
		return(-1);
	}
	else {
		push(@{ $tree }, @{ $filetree });
		return(0);
	}
}

###########################
# Miscellaneous Functions #
###########################

##
# Format a given value into acceptable C code
#
sub format_value {
	my ($value) = @_;

	return($value) if ($value =~ /^\"/);
	return("\"$value\"");
}

=not used atm

##
# Transform a widcard-contaning filename into a list of files using $dir as
# the base directory.
#
sub expand_wildcards {
	my ($dir, $file) = @_;

	# TODO FIX EXPAND_WILDCARDS()
	$file =~ /(.*?)((\/|\\)(.*)|$)/;
	my ($name, $file) = ($1, $4);
	if ($name =~ /(.*)\*(.*)/) {
		my ($left, $right) = ($1, $2);
		$left = encode_regex($left);
		$right = encode_regex($right);
		my @list = ();
		opendir(DIR, $dir ? $dir : ".") or (print "Error opening directory, \"$dir\"\n" and return(()));
		my @files = readdir(DIR);
		closedir(DIR);
		foreach (@files) {
			next if ($_ eq "." or $_ eq "..");
			my $comb = $dir ? "$dir/$_" : "$_";
			if (/^$left.*$right$/) {
				if ($file) {
					push(@list, expand_wildcards($comb, $file)) if (-d $comb);
				}
				else {
					push(@list, $comb) if (-e $comb);
				}
			}
		}
		return(@list);
	}
	else {
		my $comb = $dir ? "$dir/$name" : "$name";
		if ($file) {
			return(expand_wildcards($comb, $file)) if (-d $comb);
		}
		else {
			return(($comb)) if (-e $comb);
		}
	}
	return(());
}

=cut

##
# Find the directory containing the source for the module (or frontend) of the
# given name.  The type must be "module", "frontend", or "" (in which case
# "module" is assumed).
#
sub get_module_dir {
	my ($name, $type) = @_;

	if (!$type or ($type eq "module")) {
		$type = "modules";
	}
	elsif (!($type eq "frontend")) {
		return("");
	}

	foreach my $dir (@srcdirs) {
		return("$dir/$type/$name") if (-e "$dir/$type/$name");
	}
	return("");
}

##
# Read a line of input from a file in a standardized way, remove comments,
# and keep track of the line number.
#
sub read_line {
	my ($fd, $lineref) = @_;

	return(undef) unless (my $line = <$fd>);
	$line =~ s/(^|[^\\])\#.*$/$1/;		## Strip comments
	$line =~ s/\s*$//;
	$line =~ s/\\\#/\#/;
	while ($line =~ /(\\\s*(|\r)\n)$/) {	## If the line ends with '\' then read in the next line
		$line =~ s/\Q$1\E//;		## Removed the '\' and linebreak
		$line .= <$fd>;
		$line =~ s/\s*\#.*$//;		## Strip comments
		$$lineref++;			## Keep track of linenumbers
	}
	$line =~ s/(|\r)\n//;			## Remove line break
	$$lineref++;				## Keep track of linenumbers
	return($line);
}

##
# Insert spaces after the given string to produce a string of length $num.
#
sub align_string {
	my ($str, $num) = @_;

	my $whitespace;
	for (length($str)..$num) {
		$whitespace .= " ";
	}
	return($str . $whitespace);
}

##
# Convert a basic wildcard-contaning string into a perl-compatible regex
#
sub encode_regex {
	my ($str) = @_;
	$str =~ s/(\\|\/|\^|\.|\~|\@|\$|\||\(|\)|\[|\]|\+|\?|\{|\})/\\$1/g;
	$str =~ s/\*/\.\*/g;
	return($str);
}

##
# Generate a nicely formatted timestamp string from localtime()
#
sub get_timestamp {
	my ($sec, $min, $hour, $mday, $mon, $year, $wday) = localtime(time);
	$mon++;
	$year =~ s/^\d?//;
	return(sprintf("%02d-%02d-%02d %02d:%02d", $year, $mon, $mday, $hour, $min));
}


