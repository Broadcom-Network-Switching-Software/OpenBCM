/*! \file bcma_cli_parse_int_range.c
 *
 * This module parses a string into a range of integers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli_parse.h>

int
bcma_cli_parse_int_range(const char *str, int *start, int *end)
{
    char ch;
    int val = -1, vs = -1, ve = -1;

    if (str == NULL) {
        return -1;
    }

    do {
        ch = *str++;
        if (ch >= '0' && ch <= '9') {
            if (val == -1) {
                val = 0;
            }
            val = (val * 10) + (ch - '0');
        } else if (ch == '-') {
            if (val == -1 || vs != -1) {
                return -1;
            }
            vs = val;
            val = -1;
        } else if (ch != 0) {
            return -1;
        }
    } while (ch != 0);

    if (val == -1) {
        return -1;
    }
    if (vs == -1) {
        /* The str is in format "a" instead of "a-b" */
        vs = val;
    }
    ve = val;

    if (start) {
        *start = vs;
    }
    if (end) {
        *end = ve;
    }

    return 1;
}
