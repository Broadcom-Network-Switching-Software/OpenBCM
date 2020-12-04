/*! \file libc_isdigit.c
 *
 * SDK implementation of isdigit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_isdigit

int
sal_isdigit(int c)
{
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

#endif
