/*! \file libc_strcasecmp.c
 *
 * SDK implementation of strcasecmp (non-ANSI/ISO).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strcasecmp

int
sal_strcasecmp(const char *dest, const char *src)
{
    char dc,sc;
    int rv = 0;

    while (1) {
        dc = sal_toupper(*dest++);
        sc = sal_toupper(*src++);
        if ((rv = dc - sc) != 0 || !dc) {
            break;
        }
    }
    return rv;
}

#endif
