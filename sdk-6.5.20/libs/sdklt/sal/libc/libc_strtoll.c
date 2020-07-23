/*! \file libc_strtoll.c
 *
 * SDK implementation of strtoll.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strtoll

long long
sal_strtoll(const char *s, char **end, int base)
{
    long long n;
    int neg;
    int offs = 0;

    if (s == 0) {
        return 0;
    }

    s += (neg = (*s == '-'));

    if (base == 0 || base == 16) {
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
            base = 16;
            s += 2;
            offs = 1;
        }
        if (base == 0) {
            base = 10;
        }
    }

    for (n = 0; ((*s >= 'a' && *s < 'a' + base - 10) ||
                 (*s >= 'A' && *s < 'A' + base - 10) ||
                 (*s >= '0' && *s <= '9')); s++) {
        n = n * base + ((*s <= '9' ? *s : *s + 9) & 0xf);
        offs = 0;
    }

    if (end != 0) {
        *end = (char *)s - offs;
    }

    return (neg ? -n : n);
}

#endif
