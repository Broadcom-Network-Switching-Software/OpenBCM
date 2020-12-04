/*! \file libc_strcpy.c
 *
 * SDK implementation of strcpy.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strcpy

char *
sal_strcpy(char *dest, const char *src)
{
    char *ptr = dest;

    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';

    return dest;
}

#endif
