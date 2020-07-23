/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXC_REGRESSION_UTILS_H__
#define __DNXC_REGRESSION_UTILS_H__

#define DNXC_REGRESSION_UTILS_STRING_LEN 1024

#define DNXC_REGRESSION_UTILS_ADVANCED_PRINT(label, format, ...) \
    do { \
        dnxc_regression_utils_advanced_print(label, format, __VA_ARGS__); \
    } while(0)

void dnxc_regression_utils_print(
    char *label,
    char *data);

void dnxc_regression_utils_advanced_print(
    char *label,
    char *format,
    ...);

#endif
