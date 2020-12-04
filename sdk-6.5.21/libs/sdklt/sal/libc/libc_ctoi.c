/*! \file libc_ctoi.c
 *
 * Special string conversion function that accept 0b prefix to specify
 * binary numbers and 0 prefix for octal.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_ctoi

int
sal_ctoi(const char *s, char **end)
{
    if (s == 0) {
        return 0;
    }

    /* Parse binary as unsigned */
    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        return (int)sal_strtoul(&s[2], end, 2);
    }

    /* Parse octal as unsigned */
    if (s[0] == '0' && (s[1] >= '0' && s[1] <= '7')) {
        return (int)sal_strtoul(s, end, 8);
    }

    /* Parse hexadecimal as unsigned */
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return (int)sal_strtoul(&s[2], end, 16);
    }

    /* Parse decimal as signed */
    return (int)sal_strtol(s, end, 0);
}

#endif
