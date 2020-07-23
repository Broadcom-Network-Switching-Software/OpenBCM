/*! \file libc_islower.c
 *
 * SDK implementation of islower.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_islower

int
sal_islower(int c)
{
    if (c >= 'a' && c <= 'z') {
        return 1;
    }
    return 0;
}

#endif
