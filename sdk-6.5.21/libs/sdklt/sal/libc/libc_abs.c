/*! \file libc_abs.c
 *
 * SDK implementation of abs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_abs

int
sal_abs(int j)
{
    int pos;

    pos = j;
    if (j < 0) {
        pos = -j;
    }

    return pos;
}

#endif
