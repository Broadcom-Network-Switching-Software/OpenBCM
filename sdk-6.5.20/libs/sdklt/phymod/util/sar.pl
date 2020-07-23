#!/usr/bin/perl -w
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id$
#

use strict;
use warnings;
use File::Basename;
use IO::File;
use Data::Dumper;

my $arg;
my @files = ();
my @text = ();
my @insert_before = ();
my @insert_after = ();
my $exact = 1;
my $verbose = 0;
my $textfile;   # File contains name of functions to be changed 
my $texttoinsert; # Text to insert when function is found 
my @functions = ();
my $dryrun = 0;

while ( defined ( $arg = shift @ARGV) ) {
	if ( $arg eq '-h' || $arg eq '-help' ) {
        print_help();
		exit 0;
	}
	if ( $arg eq '-f' ) {
        while ( defined ( my $tmp = shift @ARGV ) ) {
            if ( $tmp !~ /^-/ ) {
                push ( @files, $tmp );
            } else {
                unshift ( @ARGV, $tmp );
                last;
            }
        }
        next;
    }
	if ( $arg eq '-text' ) {
		 push (@text, shift @ARGV);
		 push (@text, shift @ARGV);
		 next;
	}
    if ( $arg eq '-vague' ) {
        $exact = 0;
        next;
    }
    if ( $arg eq '-inserta' ) {
		push (@insert_after, shift @ARGV);
		push (@insert_after, shift @ARGV);
        next;
    }
    if ( $arg eq '-insertb' ) {
		push (@insert_before, shift @ARGV);
		push (@insert_before, shift @ARGV);
        next;
    }
    if ( $arg eq '-verbose' ) {
        $verbose = shift @ARGV;
        next;
    }
    if ( $arg eq '-list' ) {
        $textfile = shift @ARGV;
        $texttoinsert = shift @ARGV;
        next;
    }
    if ( $arg eq '-dryrun' ) {
        $dryrun = 1;
		next;
	}
    print "Illegal option $arg\n";
}

if ( @text || @insert_before || @insert_after ) {
    foreach my $file ( @files ) {
		my $once = 1;
        print "Processing file $file\n" if ($verbose & 0x1  );
        my $modified = 0;
        my $fhr = IO::File->new("$file",'r') || die "$! Unable to open file $file for reading\n";
        my $fhw = IO::File->new("$file.$$",'w') || die "$! Unable to open file $file.$$ for writing\n";
        while ( my $line=<$fhr> ) {
            for ( my $idx=0; $idx < @insert_before; $idx += 2 ) {
                 if ( $line =~ m/$insert_before[$idx]/ ) {
                     $modified = 1;
                     print $fhw $insert_before[$idx+1], "\n" if ( !$dryrun );
                     print $insert_before[$idx+1], "\n" if ( $dryrun );
                 }
            }
            for ( my $idx=0; $idx < @text; $idx += 2 ) {
				print "Searching for !$text[$idx]! and replace it with !$text[$idx+1]!\n" if ( $once );
                if ( $exact ) {
                    if ( $line =~ s/\b$text[$idx]\b/$text[$idx+1]/g ) {
                        $modified = 1; 
                        print "Changing this \'$_\' into this \'$line\'\n" if ( $verbose & 0x2 );
                    }
                } else { 
					if ( $line =~ s/$text[$idx]/$text[$idx+1]/g ) {
                        $modified = 1; 
                        print "Changing this \'$_\' into this \'$line\'\n" if ( $verbose & 0x2 );
                    }
                }
            }
			$once = 0;
            print $fhw $line;
            for ( my $idx=0; $idx < @insert_after; $idx += 2 ) {
                 if ( $line =~ m/$insert_after[$idx]/ ) {
                     $modified = 1;
                     print $fhw $insert_after[$idx+1], "\n" if ( !$dryrun );
                     print $insert_after[$idx+1], "\n" if ( $dryrun );
                 }
            }
        }
        close ( $fhr );
        close ( $fhw );
        if ( !$modified ) {
            unlink ("$file.$$");
            print "No match found. Original file unchanged. Removing temporary file $file.$$\n" if ( $verbose & 0x1 );
        } else {
            rename ( "$file.$$", "$file") || die "$! Unable to rename $file.$$ to $file\n" 
        }
    }
}

if ( defined $textfile ) {
    my $fhr = IO::File->new("$textfile",'r') || die "$! Unable to open file $textfile for reading\n";
    @functions = <$fhr>;
    chomp(@functions);
    close ( $fhr );
    foreach my $file ( @files ) {
        print "Processing file $file\n" if ($verbose & 0x1);
        my $modified = 0;
        my $fhr = IO::File->new("$file",'r') || die "$! Unable to open file $file for reading\n";
        my $fhw = IO::File->new("$file.$$",'w') || die "$! Unable to open file $file.$$ for writing\n";
        while ( my $line=<$fhr> ) {
            my $savedline = $line;
            for ( my $idx=0; $idx < @functions; $idx++  ) {
                next if ( $functions[$idx] =~ m/^$/ ); 
                if ( $line =~ s/($functions[$idx])/$1$texttoinsert/ ) {
                    $modified = 1; 
                    print "Changing this \'$savedline\' into this \'$line\' with search pattern \'$functions[$idx]\'\n" if ( $verbose & 0x2 );
                    last;
                }
            }
            print $fhw $line if ( !$dryrun );
            print $line if ( !$dryrun );
        }
        close ( $fhr );
        close ( $fhw );
        if ( !$modified ) {
            unlink ("$file.$$");
            print "No match found. Original file unchanged. Removing temporary file $file.$$\n" if ( $verbose & 0x1 );
        } else {
            rename ( "$file.$$", "$file") || die "$! Unable to rename $file.$$ to $file\n" 
        }
    }
}

sub print_help {
    print << "EOT"
$0 -text <text to search> <text to replace> -insert <text to search> <text to insert> -f <list of files> -vague 

This script searches all the files for specified search text pattern 
and replaces it with replacement text pattern. Note that the script 
replaces the original file with the modified file. Back up of the original
is highly recommended.

The -insertb option searches for specified text pattern and insert
the specified text before the line.
The -inserta option searches for specified text pattern and insert
the specified text after the line.
The -dryrun option shows what is being changed without modify the files
The -list option specifies a file contains a list of text to be changed 

the -text option can be used multiple times.
the -inserta and -insertb option can be used multiple times.


If the -vague option is used then the scripts will match any words
that contains the search pattern. Otherwise it will perform the exact
match.

USAGE:
$0 -text "find me" "replace me" -f *.c *.h  

EOT
}
