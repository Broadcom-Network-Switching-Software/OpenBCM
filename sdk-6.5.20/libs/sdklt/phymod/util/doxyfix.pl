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
use File::Copy;
use Data::Dumper;

my $arg;
my @files = ();
my $verbose = 0;
my $backup = 0;

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
	if ( $arg eq '-v' ) {
		$verbose = 1;
		next;
	}
	if ( $arg eq '-b' ) {
		$backup = 1;
		next;
	}
    print "Illegal option $arg\n";
}

foreach my $file ( @files ) {
  copy("$file", "$file.bak") or die "Copy failed: $!" if ( $backup );
  my $line;
  my $notfound = 0;
  my $firstparam = 0;
  print "Processing file $file\n" if ($verbose );
  my $fhr = IO::File->new("$file",'r') || die "$! Unable to open file $file for reading\n";
  my $fhw = IO::File->new("$file.$$",'w') || die "$! Unable to open file $file.$$ for writing\n";
  while ( $line=<$fhr> ) {
    if ( $line =~ m/\@param/ ) {
      print $fhw " * \@param pa \#phymod_access_t struct\n" if ( !$firstparam );
      print $fhw $line; 
      $firstparam++;
      next;
    }
    if ( $line =~ m/\@return/ && !$firstparam ) {
      print $fhw " * \@param pa \#phymod_access_t struct\n" if ( !$firstparam );
      print $fhw $line; 
      $firstparam++;
      next;
    }
    print $fhw $line;
    $firstparam = 0;
  }
  close ( $fhr );
  close ( $fhw );
  rename ( "$file.$$", "$file") || die "$! Unable to rename $file.$$ to $file\n" 
}

sub print_help {
    print << "EOT"
$0 -f <files>

This script converts DOS lf/cr to UNIX format and remove all non-ASCII caharacters. It also joins lines seperated
by \ character into a single line. 

OPTIONS:
     -v             Enable verbose
     -b             Make backup of original files
     -h             help
     -help          help


USAGE:
$0 -f *.c *.h -v -b

EOT
}
