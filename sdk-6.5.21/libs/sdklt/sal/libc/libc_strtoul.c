/*! \file libc_strtoul.c
 *
 * SDK implementation of strtoul.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strtoul

unsigned long
sal_strtoul(const char *s, char **end, int base)
{
    return (unsigned long)sal_strtoll(s, end, base);
}

#endif
