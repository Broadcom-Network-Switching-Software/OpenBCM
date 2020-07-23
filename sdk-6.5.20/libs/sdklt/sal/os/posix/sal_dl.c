/*! \file sal_dl.c
 *
 * Dynamic loader API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <dlfcn.h>

#include <sal/sal_dl.h>
#include <stdio.h>

sal_dl_t
sal_dl_open(const char *libname)
{
    sal_dl_t rv;

    rv = dlopen(libname, RTLD_LAZY);
    if (rv == NULL) {
        printf("dllopen failed error = %s\n", dlerror());
    }
    return rv;
}

void *
sal_dl_sym(sal_dl_t dlh, const char *symname)
{
    return dlsym(dlh, symname);
}

int
sal_dl_close(sal_dl_t dlh)
{
    return dlclose(dlh);
}
