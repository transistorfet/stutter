#!/usr/bin/perl
#
# Name:		config.pl
# Description:	Generate the makefiles and configuration files required for
#		building the program
#

use strict;
use IO::File;

main();
exit(0);

sub main {
	my $tree = parse_file($ARGV[0]);
	return(0) unless ($tree);

	my $decls = build_declaration_list($tree);
	return(0) unless ($decls);

	if (process_configuration($tree, $decls)) {
		return(0);
	}

	build_config_h($decls, "config.h.out");
	return(0);
}

sub build_config_h {
	my ($decls, $file) = @_;

	(my $fd = new IO::File("$file", "w")) or (print "Unable to open $file for writing\n" and return);
	foreach my $key (keys(%{ $decls })) {
		my $decl = $decls->{ $key };
		if ($decl->{'type'} eq "table") {
			my $types = build_typed_list($decl->{'decls'}, $decl->{'name'});
			emit_variable_declarations($fd, $types, $key);
		}
		elsif ($decl->{'type'} eq "") {
			# TODO i guess these could be root variables as well as #define'd constants (code)
		}
	}
	close($fd);
}

sub emit_variable_declarations {
	my ($fd, $types, $module) = @_;

# TODO finish this once you sort out how to structure the macros
	$module = uc($module);
	print $fd "#define LOAD_$module_VARIABLES()	\\\n";
	foreach my $key (keys(%{ $types })) {
		print $fd "	DECLARE_TYPE(\"$key\",	\\\n";
		foreach my $var (@{ $types->{ $key }) {
			print $fd "		ADD_VARIABLE(\"$var->[0]\", )	\\\n";	
		}
		print $fd "	)	\\\n";
	}
}

sub process_configuration {
	my ($tree, $decls) = @_;

	my $fatal_error = 0;
	my $namespace = "";
	foreach my $element (@{ $tree }) {
		if ($element->{'type'} eq "namespace") {
			$namespace = $element->{'name'};
		}
		elsif ($element->{'type'} eq "set") {
			my $name = $element->{'name'};
			my $full_name = "$namespace.$name" if ($namespace);
			my $value = get_decl($decls, $full_name);
			$value = get_decl($decls, $name) unless (defined($value));
			if (!defined($value)) {
				print "Error: $element->{'info'}: Variable \"$name\" not declared\n";
				$fatal_error = 1;
				next;
			}
			$value->{'value'} = $element->{'value'};

		}
	}
	return(-1) if ($fatal_error);
	return(0);
}

sub build_declaration_list {
	my ($tree) = @_;

	my $fatal_error = 0;
	my $namespace = "";
	my $decls = { };
	foreach my $element (@{ $tree }) {
		if ($element->{'type'} eq "namespace") {
			if ($element->{'name'} !~ /^\w(\w|\.)*?\w+$/) {
				print "Error: $element->{'info'}: Invalid namespace name \"$element->{'name'}\"\n";
				$fatal_error = 1;
				next;
			}
			$namespace = $element->{'name'};
		}
		elsif ($element->{'type'} eq "declaration") {
			my $name = $element->{'name'};
			$name = "$namespace.$name" if ($namespace and ($element->{'keyword'} ne "code"));
			my $value = {
				'name' => $name,
				'keyword' => $element->{'keyword'},
				'type' => $element->{'vartype'},
				'value' => $element->{'value'}
			};
			if (set_decl($decls, $name, $value)) {
				print "Error: $element->{'info'}: Invalid variable name \"$name\"\n";
				$fatal_error = 1;
				next;
			}
		}
	}
	return(undef) if ($fatal_error);
	return($decls);
}

sub parse_file {
	my ($file) = @_;

	$file =~ /(.*)(\/|\\)(.*?)/;
	my $dir = $1;
	my $fatal_error = 0;
	my ($linenum, $thisline) = (0, 0);

	my $tree = [ ];
	(my $fd = new IO::File("$file", "r")) or (print "Error: Unable to open $file\n" and return);
	while (defined(my $line = read_line($fd, \$linenum))) {
		($thisline = $linenum and next) if (!$line);
		if ($line =~ /^\s*include\s+(.*?)\s*$/i) {
			foreach my $file (expand_wildcards($dir, $1)) {
				push(@{ $tree }, @{ parse_file($file) });
			}
		}
		elsif ($line =~ /^\s*namespace\s+(.+?)\s*$/i) {
			my ($name) = ($1);
			push(@{ $tree }, {
				'info' => "$file:$thisline",
				'type' => "namespace",
				'name' => $name
			});
		}
		elsif ($line =~ /^\s*command\s+(\S+?)\s+(\S+?)(|\s+(.*?))\s*$/i) {
			my ($name, $func, $value) = ($1, $2, $4);
			push(@{ $tree }, {
				'info' => "$file:$thisline",
				'type' => "command",
				'name' => $name,
				'func' => $func,
				'value' => $value
			});
		}
		elsif ($line =~ /^\s*bind\s+(\S+?)\s+(\S+?)(|\s+(.*?))\s*$/i) {
			my ($name, $func, $value) = ($1, $2, $4);
			push(@{ $tree }, {
				'info' => "$file:$thisline",
				'type' => "bind",
				'name' => $name,
				'func' => $func,
				'value' => $value
			});
		}
		elsif ($line =~ /^\s*(var|const|code)\s+(\S+?)\s*(|:\s*(\S+?)\s*)=\s*(.+?)\s*$/i) {
			my ($keyword, $name, $type, $value) = ($1, $2, $4, $5);
			$type = "code" if ($keyword eq "code");
			push(@{ $tree }, {
				'info' => "$file:$thisline",
				'type' => "declaration",
				'keyword' => $keyword,
				'name' => $name,
				'vartype' => $type,
				'value' => $value
			});
		}
		elsif (($line =~ /^\s*(\S+?)\s*=\s*(.+?)\s*$/i) or ($line =~ /^\s*set\s+(\S+?)\s+(.+?)\s*$/i)) {
			my ($name, $value) = ($1, $2);
			push(@{ $tree }, {
				'info' => "$file:$thisline",
				'type' => "set",
				'name' => $name,
				'value' => $value
			});
		}
		else {
			$fatal_error = 1;
			print "Error $file:$thisline: Invalid syntax at \"$line\"\n";
		}
		$thisline = $linenum;
	}
	close($fd);
	return(undef) if ($fatal_error);
	return($tree);
}

=stop this crazy thing

sub parse_block {
	my ($fd, $dir, $file, $line_num) = @_;

	my $block = [ ];
	while (my $line = read_line($fd)) {
		next if (!$line);
		if ($line =~ /^\s*end\s*$/) {
			return($block);
		}
		elsif ($line =~ /^\s*source\s+(.+?)\s*$/i) {
			push(@{ $block }, { 'type' => "source", 'name' => expand_wildcards($dir, $1) });
		}
		elsif ($line =~ /^\s*library\s+(.+?)\s*$/i) {
			push(@{ $block }, { 'type' => "library", 'name' => expand_wildcards($dir, $1) });
		}
		elsif ($line =~ /^\s*use\s+(modules)\s*$/i) {
			push(@{ $block }, { 'type' => "use", 'name' => $1 });
		}
		else {
			print "Error $file:$$line_num: Invalid syntax at \"$line\"\n";
		}
		$$line_num++;
	}
	return($block);
}

=cut

sub get_decl {
	my ($decls, $name, $value) = @_;

	while ($name) {
		if ($name =~ /^(\w+?)(|\.(.*))$/) {
			my ($table, $rest) = ($1, $3);
			if ($rest) {
				return(undef) if (!defined($decls->{ $table }));
				$decls = $decls->{ $table }->{'decls'};
				$name = $rest;
			}
			else {
				return($decls->{ $name });
			}
		}
	}
	return(undef);
}

sub set_decl {
	my ($decls, $name, $value) = @_;

	my $complete_name = $name;
	while ($name) {
		if ($name =~ /^(\w+?)(|\.(.*))$/) {
			my ($table, $rest) = ($1, $3);
			if ($rest) {
				if (!defined($decls->{ $table })) {
					$decls->{ $table } = { "name" => $complete_name, 'type' => "table", 'decls' => { } };
				}
				$decls = $decls->{ $table }->{'decls'};
				$name = $rest;
			}
			else {
				$decls->{ $name } = $value if (defined($value));
				return(0);
			}
		}
		else {
			return(-1);
		}
	}
}

sub build_typed_list {
	my ($decls, $lead, $types) = @_;

	$types = { } unless (defined($types));
	foreach my $key (keys(%{ $decls })) {
		my $name = $key;
		$name = "$lead.$name" if ($lead);
		if ($decls->{ $key }->{'type'} eq "table") {
			build_typed_list($decls->{ $key }->{'decls'}, $name, $types);
		}
		else {
			my ($keyword, $type, $value) = ($decls->{ $key }->{'keyword'}, $decls->{ $key }->{'type'}, $decls->{ $key }->{'value'});
			if ($keyword eq "code"); {
				$type = "code";
				$name = $key;
			}
			$types->{ $type } = [ ] unless (defined($types->{ $type }));
			push($types->{ $type }, [ $name, $value, ($keyword eq "const") ? 1 : 0 ];
		}
	}
	return($types);
}

sub expand_wildcards {
	my ($dir, $file) = @_;

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

sub read_line {
	my ($fd, $lineref) = @_;

	return(undef) unless (my $line = <$fd>);
	$line =~ s/\s*\#.*$//;			## Strip comments
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

sub encode_regex {
	my ($str) = @_;
	$str =~ s/(\\|\/|\^|\.|\~|\@|\$|\||\(|\)|\[|\]|\+|\?|\{|\})/\\$1/g;
	$str =~ s/\*/\.\*/g;
	return($str);
}

sub get_timestamp {
	my ($sec, $min, $hour, $mday, $mon, $year, $wday) = localtime(time);
	$mon++;
	$year =~ s/^\d?//;
	return(sprintf("%02d-%02d-%02d %02d:%02d", $year, $mon, $mday, $hour, $min));
}


