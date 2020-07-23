/*! \file libc_strnchr.c
 *
 * SDK implementation of strnchr.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strnchr

char *
sal_strnchr(const char *dest, int c, size_t cnt)
{
    while (*dest && (cnt > 0)) {
        if (*dest == c) {
            return (char *)dest;
        }
        dest++;
        cnt--;
    }
    return NULL;
}

#endif
