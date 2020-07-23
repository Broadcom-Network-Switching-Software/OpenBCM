/*! \file libc_atoi.c
 *
 * SDK implementation of atoi.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_atoi

int
sal_atoi(const char *s)
{
    return (int)sal_strtol(s, NULL, 10);
}

#endif
