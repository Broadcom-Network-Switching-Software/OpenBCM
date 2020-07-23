/*! \file libc_isalpha.c
 *
 * SDK implementation of isalpha.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_isalpha

int
sal_isalpha(int c)
{
    return (sal_islower(c) || sal_isupper(c));
}

#endif
