/*! \file libc_strlen.c
 *
 * SDK implementation of strlen.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strlen

size_t
sal_strlen(const char *str)
{
    size_t cnt = 0;

    while (*str) {
        str++;
        cnt++;
    }

    return cnt;
}

#endif
