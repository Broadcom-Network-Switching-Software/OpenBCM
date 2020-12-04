/*! \file libc_memcmp.c
 *
 * SDK implementation of memcmp.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_memcmp

int
sal_memcmp(const void *s1, const void *s2, size_t cnt)
{
    const unsigned char *d;
    const unsigned char *s;

    d = (const unsigned char *)s1;
    s = (const unsigned char *)s2;

    while (cnt) {
        if (*d < *s) {
            return -1;
        }
        if (*d > *s) {
            return 1;
        }
        d++;
        s++;
        cnt--;
    }

    return 0;
}

#endif
