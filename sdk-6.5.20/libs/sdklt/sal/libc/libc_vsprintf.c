/*! \file libc_vsprintf.c
 *
 * SDK implementation of vsprintf.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_vsprintf

int
sal_vsprintf(char *buf, const char *fmt, va_list ap)
{
    return sal_vsnprintf(buf, SAL_VSNPRINTF_X_INF, fmt, ap);
}

#endif
