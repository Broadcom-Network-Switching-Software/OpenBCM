/*! \file libc_strncasecmp.c
 *
 * SDK implementation of strncasecmp (non-ANSI/ISO).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strncasecmp

int
sal_strncasecmp(const char *dest, const char *src, size_t cnt)
{
    char dc, sc;
    int rv = 0;

    while (cnt) {
        dc = sal_toupper(*dest++);
        sc = sal_toupper(*src++);
        if ((rv = dc - sc) != 0 || !dc) {
            break;
        }
        cnt--;
    }
    return rv;
}

#endif
