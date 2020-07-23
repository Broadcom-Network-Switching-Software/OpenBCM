/*! \file libc_strupr.c
 *
 * SDK implementation of strupr (non-ANSI/ISO).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strupr

void
sal_strupr(char *str)
{
    while (*str) {
        *str = sal_toupper(*str);
        str++;
    }
}

#endif
