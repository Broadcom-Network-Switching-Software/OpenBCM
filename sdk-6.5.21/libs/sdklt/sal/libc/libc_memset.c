/*! \file libc_memset.c
 *
 * SDK implementation of memset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_memset

void *
sal_memset(void *dest, int c, size_t cnt)
{
    unsigned char *d;

    d = dest;

    while (cnt) {
        *d++ = (unsigned char)c;
        cnt--;
    }

    return d;
}

#endif
