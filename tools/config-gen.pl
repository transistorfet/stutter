#!/usr/bin/perl
#
# Script Name:	config-gen.pl
# Date:		2006/12/08
# Description:	Generates the default assignments block in the config-def.h
#		files using the #defines in the same file.
#

main();
exit(0);

sub main {
	if (scalar(@ARGV) != 1) {
		print "Usage: config-gen.pl <config-file>\n";
		exit(-1);
	}

	my @defines = ();
	my $file = $ARGV[0];
	my $tmpfile = "$ARGV[0].tmp";
	open(FILE, $file) or (print "Error: \"$file\" not found\n" and exit(-1));
	open(TMP, ">$tmpfile") or (print "Error: Unable to open \"$tmpfile\" for writing\n" and exit(-1));
	while ($line = <FILE>) {
		if ($line =~ /^\s*\#define\s+(DEFAULT_[a-zA-Z0-0_]+)(\(.*\)|)(.*?)\s*(\r|)\n$/) {
			push(@defines, [ $1, $2 ]);
			print TMP $line;
		}
		elsif ($line =~ /^\s*\/\*\*\* START OF DEFAULT ASSIGNMENTS \*\*\*\/\s*((\r|)\n)$/) {
			my $delim = $1;
			print TMP $line;
			foreach $define (@defines) {
				my $name = $define->[0];
				$name =~ s/DEFAULT_//;
				my $params = $define->[1];
				$params =~ s/\.\.\./__VA_ARGS__/g;
				print TMP "#ifndef $name$delim";
				print TMP "#define $name$define->[1] \\$delim";
				print TMP "\t$define->[0]$params$delim";
				print TMP "#endif$delim";
			}
			while (($line = <FILE>) and !($line =~ /^\s*\/\*\*\* END OF DEFAULT ASSIGNMENTS \*\*\*\/\s*(\r|)\n$/)) { }
			unless ($line) {
				print "Error: Closing default assignments block comment not found\n";
				close(FILE);
				close(TMP);
				unlink($tmpfile);
				exit(-1);
			}
			print TMP $line;
		}
		else {
			print TMP $line;
		}
	}
	close(TMP);
	close(FILE);

	unlink($file);
	rename($tmpfile, $file);
	return(0);
}

