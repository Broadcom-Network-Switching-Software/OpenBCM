/*! \file libc_strtol.c
 *
 * SDK implementation of strtol.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strtol

long
sal_strtol(const char *s, char **end, int base)
{
    return (long)sal_strtoll(s, end, base);
}

#endif
