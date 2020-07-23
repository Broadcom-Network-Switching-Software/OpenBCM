/** \file dnx_cint_regression_utils.c
 *
 * Expose dnxc regression utils, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_COMMON

typedef int dnx_cint_regression_utils_not_empty; /* Make ISO compilers happy. */

#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
/* { */
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <soc/dnxc/dnxc_regression_utils.h>

/**
 * in macro CINT_FWRAPPER_CREATE_XXX:
 * the first line represent the return value
 * the second line represent the API name
 * each line represents a input parameter.
 */
CINT_FWRAPPER_CREATE_VP2(dnxc_regression_utils_print,
                         char *, char, label, 1, 0,
                         char *, char, data, 1, 0);


static cint_function_t __cint_dnxc_regression_utils_functions[] =
    {
        CINT_FWRAPPER_NENTRY("dnxc_regression_utils_print", dnxc_regression_utils_print),
        CINT_ENTRY_LAST
    };

cint_data_t dnxc_cint_regression_utils =
{
    NULL,
    __cint_dnxc_regression_utils_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif /*defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)*/
