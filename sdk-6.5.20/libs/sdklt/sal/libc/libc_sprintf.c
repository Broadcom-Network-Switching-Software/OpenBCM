/*! \file libc_sprintf.c
 *
 * SDK implementation of sprintf.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_sprintf

int
sal_sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsnprintf(buf, SAL_VSNPRINTF_X_INF, fmt, ap);
    va_end(ap);

    return r;
}

#endif
