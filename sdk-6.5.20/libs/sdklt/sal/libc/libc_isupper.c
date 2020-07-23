/*! \file libc_isupper.c
 *
 * SDK implementation of isupper.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_isupper

int
sal_isupper(int c)
{
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

#endif
