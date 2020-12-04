/*! \file sal_assert.c
 *
 * Default POSIX output function for failed assert().
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>

#include <sal/sal_assert.h>

static void
default_assert(const char *expr, const char *file, int line)
{
    printf("ERROR: Assertion failed: (%s) at %s:%d\n", expr, file, line);
    abort();
}

static sal_assert_fn_t assert_fn = default_assert;

void
sal_assert_set(sal_assert_fn_t fn)
{
    assert_fn = (fn) ? fn : default_assert;
}

void
sal_assert(const char *expr, const char *file, int line)
{
    assert_fn(expr, file, line);
}
