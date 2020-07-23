/*! \file bcma_cli_parse_bit_list.c
 *
 * This module parses the bit list string and restore into a bit array.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

int
bcma_cli_parse_bit_list(const char *str, int max_bits, SHR_BITDCL *bit_array)
{
    int bnum = -1, bstart = -1;
    char ch;
    const char *cptr = str;

    if (str == NULL || bit_array == NULL || max_bits <= 0) {
        return -1;
    }

    SHR_BITCLR_RANGE(bit_array, 0, max_bits);

    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            if (bnum < 0) {
                bnum = 0;
            }
            bnum = (bnum * 10) + (ch - '0');
            if (bnum >= max_bits) {
                /* buffer is not enough */
                return -1;
            }
        } else {
            if (bstart >= 0) {
                if (bstart > bnum) {
                    return -1;
                }
                while (bstart < bnum) {
                    SHR_BITSET(bit_array, bstart);
                    bstart++;
                }
                bstart = -1;
            }
            if (ch == ',' || ch == 0) {
                if (bnum < 0) {
                    return -1;
                }
                SHR_BITSET(bit_array, bnum);
                bnum = -1;
            } else if (ch == '-') {
                if (bnum < 0) {
                    return -1;
                }
                bstart = bnum;
                bnum = -1;
            } else {
                return -1;
            }
        }
    } while (ch != 0);

    return 0;
}
