/*! \file libc_strrchr.c
 *
 * SDK implementation of strrchr.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strrchr

char *
sal_strrchr(const char *dest, int c)
{
    char *ret = NULL;

    while (*dest) {
        if (*dest == c) {
            ret = (char *)dest;
        }
        dest++;
    }

    return ret;
}

#endif
