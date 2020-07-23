/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SDK implementation of strcat.
 */

#include <sal/sal_libc.h>

#ifndef sal_strcat

char *
sal_strcat(char *dest, const char *src)
{
    char *ptr = dest;

    while (*ptr) {
        ptr++;
    }
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';

    return dest;
}

#endif
