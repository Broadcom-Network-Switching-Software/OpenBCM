/*! \file shr_exception.c
 *
 * Exception handler support functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_exception.h>

static shr_exception_handler_f exception_handler;

void
shr_exception_handler_set(shr_exception_handler_f func)
{
    exception_handler = func;
}

void
shr_exception(int unit, int err_code,
              const char *file, int line, const char *func)
{
    if (exception_handler) {
        exception_handler(unit, err_code, file, line, func);
    }
}

