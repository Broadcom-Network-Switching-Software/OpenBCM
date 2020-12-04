/*! \file bcma_cli_parse_str.c
 *
 * This module parses a string into an unsigned long interger.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

char *
bcma_cli_parse_str(const char *str, const char *incl, const char *excl,
                   char **endptr)
{
    char *str2, *ptr;

    if (str == NULL) {
        return NULL;
    }

    str2 = sal_strdup(str);
    if (str2 == NULL) {
        return NULL;
    }
    ptr = str2;

    while (*str != '\0') {
        if (incl && sal_strchr(incl, *str)) {
            *ptr = *str;
            ptr++;
            str++;
        } else if (excl && sal_strchr(excl, *str)) {
            str++;
        } else {
            break;
        }
    }

    if (*ptr != '\0') {
        *ptr = '\0';
    }
    if (endptr) {
        *endptr = (char *)str;
    }

    return str2;
}
