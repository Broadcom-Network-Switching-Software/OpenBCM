/*! \file libc_strlwr.c
 *
 * SDK implementation of strlwr (non-ANSI/ISO).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strlwr

void
sal_strlwr(char *str)
{
    while (*str) {
        *str = sal_tolower(*str);
        str++;
    }
}

#endif
