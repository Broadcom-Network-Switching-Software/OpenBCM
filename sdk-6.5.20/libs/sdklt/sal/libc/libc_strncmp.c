/*! \file libc_strncmp.c
 *
 * SDK implementation of strncmp.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strncmp

int
sal_strncmp(const char *dest, const char *src, size_t cnt)
{
    int rv = 0;

    while (cnt) {
        if ((rv = *dest - *src++) != 0 || !*dest++) {
            break;
        }
        cnt--;
    }
    return rv;
}

#endif
