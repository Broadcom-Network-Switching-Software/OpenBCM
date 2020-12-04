/*! \file libc_toupper.c
 *
 * SDK implementation of toupper.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_toupper

char
sal_toupper(char c)
{
    if (c >= 'a' && c <= 'z') {
        c -= 32;
    }
    return c;
}

#endif
