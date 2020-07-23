#!/usr/bin/perl
# $Id: lmsc.pl,v 1.26 2012/10/22 02:04:48 alai Exp $
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 
################################################################################
#
# lmsc.pl
#
# Linux Module Symbol Checker
#
# Look for all undefined or non-exported symbols in the given set of files. 
# 


sub help
  {
    print "Linux Module Symbol Checker\n\n"; 
    print "  Check for undefined symbols between a group modules.\n";
    print "  Can be used to detect link errors which will occur at insmod time\n"; 
    print "  and verify the expected link dependencies between them.\n"; 
    print "  Any symbols which cannot be linked between the set of given files\n"; 
    print "  will be displayed.\n\n"; 
    print "Usage: lmsc.pl [option=value]* <file0> <file1> <file2> ...\n"; 
    print "       lmsc.pl [option=value]* <file0>,<file1>,<file2> <file1,file10> ..\n"; 
    print "  Options:\n"; 
    print "    kernel=<filename>    This image is included in all file sets.\n"; 
    print "    ferror=<num>         Specify exit code when files cannot be found.\n";
    print "                         Allows recovery in makefiles.\n";
    exit(2); 
  }
      
    
use File::Basename; 

#
# File aliases. Mainly used to reference kernel images
#
my $KERN_PATH="/projects/ntsw-sw/linux/kernels";
my $KERN_BASE="$KERN_PATH/vmlinux";
my $KERN_TOOLS_PATH="/projects/ntsw-tools/linux/kernels";
my $KERN_TOOLS_BASE="$KERN_TOOLS_PATH/vmlinux";
my $KERN_REL=`uname -r`; 

# Strip trailing whitespace
$KERN_REL =~ s/\s+$//;

my %ALIASES = (
    "bmw-2_6", "$KERN_BASE.bmw-2.6wrs",
    "gto-2_6", "$KERN_BASE.gto-2.6wrs",
    "gto-2_6-wr30", "$KERN_BASE.gto-2.6wrs30",
    "gto", "$KERN_BASE.gto",
    "gtr-3_7", "$KERN_PATH/wrx/xlp316/vmlinux",
    "gtx-2_6", "$KERN_BASE.gtx-2.6wrl",
    "iproc-2_6", "$KERN_TOOLS_PATH/../iproc_ldks/ldk26/iproc/kernel/linux-custom/vmlinux",
    "iproc-3_6", "$KERN_TOOLS_PATH/../iproc_ldks/ldk36/iproc/kernel/linux-custom/vmlinux",
    "iproc-4_4", "$KERN_TOOLS_PATH/../iproc_ldks/xldk40/XLDK/kernel/linux/vmlinux",
    "iproc", "$KERN_TOOLS_PATH/../iproc_ldks/iproc/XLDK32/kernel/linux/vmlinux",
    "iproc_64", "$KERN_TOOLS_PATH/../iproc_ldks/iproc/XLDK64/kernel/linux/vmlinux",
    "jag", "$KERN_BASE.jag",
    "jag-2_6", "$KERN_BASE.jag-2.6wrs",
    "keystone-2_6", "$KERN_BASE.key_be-26wrs",
    "keystone_le-2_6", "$KERN_BASE.key_le-26wrs",
    "nsx", "$KERN_TOOLS_BASE.nsx",
    "nsx-2_6", "$KERN_BASE.nsx-2.6",
    "nsx_wrl-2_6", "$KERN_BASE.nsx-2.6wrs",
    "raptor", "$KERN_BASE.raptor",
    "raptor-2_6", "$KERN_BASE.raptor-2.6wrs",
    "wrx-3_7", "$KERN_PATH/wrx/xlp208/vmlinux",
    "x86-generic-2_6", "/lib/modules/$KERN_REL/source/vmlinux",
    "x86-generic_64-2_6", "/lib/modules/$KERN_REL/source/vmlinux",
    "x86-smp_generic-2_6", "/lib/modules/$KERN_REL/source/vmlinux",
    "x86-smp_generic_64-2_6", "/lib/modules/$KERN_REL/source/vmlinux",
);
               
#
# All of the sets of files which require dependency/link checking
#
my @FILESETS; 

#
# All defined symbols, by file
my %DEFINED; 

#
# All undefined symbols, by file
#
my %UNDEFINED;


#
# Simple command line options
#
my @FILES; 
my $KERNEL; 
my $FERROR = -1; 

if(@ARGV==0) {
  help(); 
}

foreach (@ARGV) {
  if(/kernel=(.*)/) {
    # Specifies a kernel name that should be prepended to all sets
    $KERNEL = $1; 
    next;
  }
  if(/ferror=(.*)/) {
    # Override exit code if file does not exist
    $FERROR=$1; 
    next; 
  }
  if(/^(--help|-h|--h|help)/) {
    help(); 
  }

  push @FILES, $_; 
}

#
# A single set of files can be specified as "file0 file1 file2..." on the command line. 
# Multiple sets of files can be specified as "file0,file1,file2 file0,file2,file3..." on the command line. 
# 

if((grep { /,/ } @FILES) == 0) {
  # Assume all arguments are in the same fileset  
  push @FILESETS, join(",", @FILES); 
}
else {
  # Each argument represents a separate fileset
  push @FILESETS, @FILES;   
}

#
# Add the kernel image if specified
#
if(defined($KERNEL)) {
  map { $_ = "$KERNEL,$_"; } @FILESETS; 
}



#
# Load all file symbols
#
load_all_symbols(); 


#
# Check all filesets and set exit code
#
exit(check_all()); 


################################################################################

#
# Check for filename aliases
#
sub alias
  {
    my $file = shift; 
    return defined($ALIASES{$file}) ? $ALIASES{$file} : $file; 
  }
    

#
# Check All Filesets
#
sub check_all
  {
    my $count; 
    foreach my $fs (@FILESETS) {
      $count += check_fs(split /,/,$fs); 
    }
    return $count == 0 ? 0 : -1; 
  }

#
# Check a single fileset
#
sub check_fs
  {
    my @files = @_; 

    my @defined; 

    # Get all defined symbols for this fileset
    foreach (@files) {
      push @defined, @{$DEFINED{alias($_)}}; 
    }
    
    # Check 
    my @pfiles = map { basename($_); } @files; 
    printf("Link Check: @pfiles: "); 
    my $count = 0; 

    foreach my $file (@files) {
      foreach my $sym (@{$UNDEFINED{$file}}) {
        if($sym =~ /^__crc_(.+)/) {
          $sym = $1; 
        } 

        if((grep { $_ eq $sym } @defined) == 0) {
          printf("\n    *** %s: $sym", basename($file)); 
          $count++;
        }
      }
    }

    if($count == 0) {
      printf("OK"); 
    }
    printf("\n"); 
    return $count; 
  }


#
# Load all defined and undefined symbols for a file
#
sub load_all_symbols
  {    
    foreach my $fs (@FILESETS) {
      foreach my $file (split /,/, $fs) {
        
        #
        # Check for aliases
        #
        $file = alias($file); 
        
        # Load the defined symbols if we haven't already
        if(!defined($DEFINED{$file})) {

          #
          # File exist?
          #
          if(!(-e $file)) {
            printf("Warning: Cannot find file '$file'. Link check not performed.\n"); 
            exit($FERROR); 
          }

          #
          # Get all defined symbols
          #   
          my @defined = nm($file, "--defined-only"); 
    
          # 
          # If these are 2.6 modules, only the ksymtab symbols are actually available for linking. 
          #
          if(grep { /__ksymtab_/ } @defined) {
            @defined = grep { s/__ksymtab_// } @defined; 
          }
          
          $DEFINED{$file} = \@defined; 
        }

        # Load the undefined symbols if we haven't already
        if(!defined($UNDEFINED{$file})) {
          my @undefined = nm($file, "--undefined-only"); 
          $UNDEFINED{$file} = \@undefined;        
        }
      }
    }
  }

#
# Use 'nm' to retrieve symbols
#
sub nm
  {
    my ($file, @flags) = @_; 

    my @symbols; 
    
    foreach my $nm_output (`/tools/bin/nm @flags $file`) {
      my @lines = split(/\n/, $nm_output);
      foreach(@lines) {
        @str = split / / ;
        $symbol = $str[$#str];
        
        # Magic symbols
        if($symbol =~ /(__start|__stop)___kallsyms/) {
          next;
        }
        if($symbol =~ /__this_module/) {
          next;
        }
        if($symbol =~ /__stack_chk_/) {
          next;
        }
        if($symbol =~ /mcount/) {
          next;
        }
        push @symbols, $symbol;
      }
    }
    chomp @symbols;
    return @symbols;
  }

