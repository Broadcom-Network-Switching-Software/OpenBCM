/*! \file libc_snprintf.c
 *
 * SDK implementation of snprintf.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_snprintf

int
sal_snprintf(char *buf, size_t bufsz, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = sal_vsnprintf(buf, bufsz, fmt, ap);
    va_end(ap);

    return r;
}

#endif
