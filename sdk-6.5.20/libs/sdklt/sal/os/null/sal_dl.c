/*! \file sal_dl.c
 *
 * Dynamic loader API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_dl.h>

sal_dl_t
sal_dl_open(const char *libname)
{
    return NULL;
}

void *
sal_dl_sym(sal_dl_t dlh, const char *symname)
{
    return NULL;
}

int
sal_dl_close(sal_dl_t dlh)
{
    return -1;
}
