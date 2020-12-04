/*! \file libc_strncpy.c
 *
 * SDK implementation of strncpy.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strncpy

char *
sal_strncpy(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;

    while (*src && (cnt > 0)) {
        *ptr++ = *src++;
        cnt--;
    }

    while (cnt > 0) {
        *ptr++ = 0;
        cnt--;
    }

    return dest;
}

#endif
