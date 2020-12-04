/*! \file bcmltd_bitop_str_decode.c
 *
 * Encode string into bit array.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_bitop.h>

/*
 * Function: bcmltd_bitop_str_decode
 *
 * decode a string in hex format into a bitmap
 * returns 0 on success, -1 on error
 *
 * The string can be more than 32 bits worth of
 * data if it is in hex format (0x...).  If not
 * hex, it is treated as a 64 bit value
 */

int
bcmltd_bitop_str_decode(const char *str,
        BCMLTD_BITDCL *dest, int max_bitdcl)
{
    const char *e;
    uint32_t v;
    int bit;
    int max_bits = BCMLTD_BITWID * max_bitdcl;


    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        bcmltd_bitop_range_clear(dest, 0, max_bits);
        /* Get end of string */
        str += 2;
        for (e = str; *e; e++)
            ;
        e -= 1;
        /* Back up to beginning of string, setting bits as we go */
        bit = 0;
        while (e >= str) {
            if (*e >= '0' && *e <= '9') {
                v = *e - '0';
            } else if (*e >= 'a' && *e <= 'f') {
                v = *e - 'a' + 10;
            } else if (*e >= 'A' && *e <= 'F') {
                v = *e - 'A' + 10;
            } else {
                /* Invalid hex digits */
                return -1;
            }
            e -= 1;
            /* Now set a nibble's worth of bits */
            if ((v & 1) && bit < max_bits) {
                BCMLTD_BIT_SET(dest, bit);
            }
            bit += 1;
            if ((v & 2) && bit < max_bits) {
                BCMLTD_BIT_SET(dest, bit);
            }
            bit += 1;
            if ((v & 4) && bit < max_bits) {
                BCMLTD_BIT_SET(dest, bit);
            }
            bit += 1;
            if ((v & 8) && bit < max_bits) {
                BCMLTD_BIT_SET(dest, bit);
            }
            bit += 1;
        }
    } else {
        v = 0;
        while (*dest >= '0' && *dest <= '9') {
            v = v * 10 + (*dest++ - '0');
        }
        if (*dest != '\0') {
            /* Invalid decimal digits */
            return -1;
        }
        bit = 0;
        while (v) {
            if ((v & 1) && bit < max_bits) {
                BCMLTD_BIT_SET(dest, bit);
            }
            v >>= 1;
            bit += 1;
        }
    }
    return 0;
}
