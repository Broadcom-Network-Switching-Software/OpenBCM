/*! \file libc_strncat.c
 *
 * SDK implementation of strncat.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strncat

char *
sal_strncat(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;

    while (*ptr) {
        ptr++;
    }
    while (*src && (cnt > 0)) {
        *ptr++ = *src++;
        cnt--;
    }
    *ptr = '\0';

    return dest;
}

#endif
