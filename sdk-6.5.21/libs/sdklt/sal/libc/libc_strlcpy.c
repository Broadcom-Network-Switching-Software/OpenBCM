/*! \file libc_strlcpy.c
 *
 * SDK implementation of strlcpy (non-ANSI/ISO).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strlcpy

size_t
sal_strlcpy(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;
    size_t copied = 0;

    while (*src && (cnt > 1)) {
        *ptr++ = *src++;
        cnt--;
        copied++;
    }
    *ptr = '\0';

    return copied;
}

#endif

