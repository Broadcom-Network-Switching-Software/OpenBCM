/*! \file libc_memmove.c
 *
 * SDK implementation of memmove.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_memmove

void *sal_memmove(void *dest, const void *src, size_t cnt)
{
    unsigned char *d;
    const unsigned char *s;

    if (cnt > 0 && dest != src) {
        d = (unsigned char *)dest;
        s = (const unsigned char *)src;
        if ((uintptr_t)dest < (uintptr_t)src) {
            while (cnt--) {
                *d++ = *s++;
            }
        } else {
            d += cnt - 1;
            s += cnt - 1;
            while (cnt--) {
                *d-- = *s--;
            }
        }
    }
    return dest;
}

#endif
