/*! \file libc_isalnum.c
 *
 * SDK implementation of isalnum.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_isalnum

int
sal_isalnum(int c)
{
    return (sal_isalpha(c) || sal_isdigit(c));
}

#endif
