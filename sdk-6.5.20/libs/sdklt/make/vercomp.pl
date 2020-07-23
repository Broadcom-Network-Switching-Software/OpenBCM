#!/usr/bin/env perl
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

@ver1 = split(/\./, @ARGV[0]);
$op = @ARGV[1];
@ver2 = split(/\./, @ARGV[2]);

if (@ver1 == 0 || $op !~ /^(eq|ne|lt|gt|le|ge)$/ || @ver2 == 0) {
    printf "\n";
    printf "DESCRIPTION\n";
    printf "Compare version numbers and return success if true.\n\n";
    printf "SYNTAX\n";
    printf "vercomp x[.y[.z] ...] eq|ne|lt|gt|le|ge a[.b[.c] ...]\n\n";
    printf "EXAMPLES\n";
    printf "vercomp 1.2.1 gt 1.2\n";
    printf "vercomp 2.9.3.12 ge 2.1\n\n";
    exit (2);
}

for ($ix = 0; $ix < @ver1 || $ix < @ver2; $ix++) {
    exit ($op !~ /^(lt|le|ne)$/) if ($ver1[$ix] < $ver2[$ix]);
    exit ($op !~ /^(gt|ge|ne)$/) if ($ver1[$ix] > $ver2[$ix]);
}
exit ($op !~ /^(ge|le|eq)$/);
