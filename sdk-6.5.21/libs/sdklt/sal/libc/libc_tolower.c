/*! \file libc_tolower.c
 *
 * SDK implementation of tolower.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_tolower

char
sal_tolower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c;
}

#endif
