/*! \file libc_strcmp.c
 *
 * SDK implementation of strcmp.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strcmp

int
sal_strcmp(const char *dest, const char *src)
{
    int rv = 0;

    while (1) {
        if ((rv = *dest - *src++) != 0 || !*dest++) {
            break;
        }
    }
    return rv;
}

#endif
