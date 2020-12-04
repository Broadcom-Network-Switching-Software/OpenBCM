#!/usr/bin/perl
#
# $Id: stripfix.pl,v 1.2 2011/04/12 09:05:28 sraj Exp $
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Perl version of the "stripfix.c" program
#
# In Tornado tools, stripppc and objcopyppc corrupt the ELF program header
# and require the use of the "stripfix" kludge.  Alternately, Solaris'
# strip works on PPC binaries also (/usr/ccs/bin/strip).
#

use Fcntl;

($prog = $0) =~ s,.*/,,;

($#ARGV == 3) ||
    die "Usage: $prog <unstripped-file> <stripped-file> <byte-offset> <word-count>\n" .
        "\tThis utility extracts <word-count> words from <unstripped-file>\n" .
        "\tstarting at offset <byte-offset>, and pastes them into the same\n" .
        "\tlocation in <stripped-file>.\n";

($unstripped, $stripped, $offset, $words) =
    ($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);

#
# Read the interesting bytes from the unstripped input file
#
sysopen(INFILE, $unstripped, O_RDONLY, 0) ||
    die "$prog: input file $unstripped: cannot open: $!\n";
binmode INFILE;
sysseek(INFILE, $offset, SEEK_SET) ||
    die "$prog: input file $unstripped: seek failed: $!\n";
$len = sysread(INFILE, $data, $words * 4);
defined($len) ||
    die "$prog: input file $unstripped: read failed: $!\n";
($len == $words * 4) ||
    die "$prog: input file $unstripped: read failed: returned $len bytes\n";
close(INFILE) ||
    die "$prog: input file $unstripped: close failed: $!\n";

#
# Now overwite the corresponding bytes in the stripped output file
#
sysopen(OUTFILE, $stripped, O_WRONLY, 0) ||
    die "$prog: output file $stripped: cannot open: $!\n";
binmode OUTFILE;
sysseek(OUTFILE, $offset, SEEK_SET) ||
    die "$prog: output file $stripped: seek failed: $!\n";
$len = syswrite(OUTFILE, $data, $words * 4);
defined($len) ||
    die "$prog: output file $stripped: write failed: $!\n";
($len == $words * 4) ||
    die "$prog: output file $stripped: write failed: only $len bytes written\n";
close(OUTFILE) ||
    die "$prog: output file $stripped: close failed: $!\n";

exit 0;
