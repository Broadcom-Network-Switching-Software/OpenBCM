/*! \file libc_strchr.c
 *
 * SDK implementation of strchr.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strchr

char *
sal_strchr(const char *dest, int c)
{
    while (*dest) {
        if (*dest == c) {
            return (char *)dest;
        }
        dest++;
    }
    return NULL;
}

#endif
