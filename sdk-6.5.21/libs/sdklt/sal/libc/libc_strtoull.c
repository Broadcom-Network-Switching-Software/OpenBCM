/*! \file libc_strtoull.c
 *
 * SDK implementation of strtoull.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_strtoull

unsigned long long
sal_strtoull(const char *s, char **end, int base)
{
    return (unsigned long long)sal_strtoll(s, end, base);
}

#endif
