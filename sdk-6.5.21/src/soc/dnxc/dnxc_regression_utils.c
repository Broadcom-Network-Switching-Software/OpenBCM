
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <assert.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnxc/dnxc_regression_utils.h>

#define BSL_LOG_MODULE BSL_LS_SOCDNX_COMMON

void
dnxc_regression_utils_print(
    char *label,
    char *data)
{
    LOG_CLI(("RegrresionDBAddContent:{%s:%s}\n", label, data));
}

void
dnxc_regression_utils_advanced_print(
    char *label,
    char *format,
    ...)
{
    va_list args;
    char str_to_print[DNXC_REGRESSION_UTILS_STRING_LEN] = { 0 };

    va_start(args, format);
    sal_vsnprintf(str_to_print, DNXC_REGRESSION_UTILS_STRING_LEN - 1, format, args);
    dnxc_regression_utils_print(label, str_to_print);
    va_end(args);
}
