/*! \file bcma_io_term_vprintf.c
 *
 * Output function for BSL console sink.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdarg.h>
#include <stdio.h>

#include <bcma/io/bcma_io_term.h>

int
bcma_io_term_vprintf(const char *fmt, va_list varg)
{
    int rv;

    rv = vprintf(fmt, varg);
    fflush(stdout);

    return rv;
}
