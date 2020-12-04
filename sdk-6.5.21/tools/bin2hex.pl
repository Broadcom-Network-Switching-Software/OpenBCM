#!/usr/bin/perl
# $Id: bin2hex.pl,v 1.2 2011/04/12 09:05:28 sraj Exp $
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 
# bin2hex.pl
# Convert Binary file to ASCII/HEX
#

    ($#ARGV == 1) ||
    die "Usage: $0 <binary-file> <Ascii_Hex-file>\n" .
        "\tThis utility converts Binary in <binary-file>\n" .
        "\tand converts to Ascii/Hex  and writes them into <Ascii_Hex-file>.\n";

    my $in_file=shift;
    my $out_file=shift;

    print STDOUT "Converting ", $in_file, " to  ", $out_file, " ...";

    $RECSIZE = 1; # size of record, in bytes
    open(FH, "<$in_file") || die "can't read $in_file : $!";
    open(FHO, ">$out_file") || die "can't write $out_file : $!";

    binmode FH;

    $count = 0;
    while ( ! eof(FH) ) {
        if (read(FH, $byte0, $RECSIZE) != $RECSIZE) {
            $byte0 = "\000";
        }
        
        $byte0 = ord($byte0);
        if ($count == 0) {
            print FHO sprintf("  0x%02x", $byte0);
            $count = 2;
        } else {
            if ($count == 16) {
                print FHO sprintf(", 0x%02x\n", $byte0);
                $count = 1;
            } else {
                print FHO sprintf(", 0x%02x", $byte0);
                $count++;
            }
        }
    }
    close FH;  
    close FHO;  

    print STDOUT "  done\n";
