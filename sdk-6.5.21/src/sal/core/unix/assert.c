/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>


#include <soc/debug.h>


#include <sal/core/libc.h>
#include <sal/core/boot.h>

/*
 * Function:
 *	_default_assert
 * Purpose:
 *	Default assertion routine used if not assigned by
 *	a call to sal_assert_set().
 */

void
_default_assert(const char *expr, const char *file, int line)
{
    printf("ERROR: Assertion failed: (%s) at %s:%d\n",
	   expr, file, line);
    abort();
}

/*
 * Function:
 *	sal_assert_set
 * Purpose:
 *	Set new assertion handler routine.
 * Parameters:
 *	f - assertion handler function
 */

static sal_assert_func_t _assert = _default_assert;

void
sal_assert_set(sal_assert_func_t f)
{
    _assert = (f) ? f : _default_assert;
}

/*
 * Function:
 *	_sal_assert
 * Purpose:
 *	Main assertion routine
 * Notes:
 *	Normally #define'd to assert().
 */

/* coverity[+kill] */
void
_sal_assert(const char *expr, const char *file, int line)
{
    _assert(expr, file, line);
}
