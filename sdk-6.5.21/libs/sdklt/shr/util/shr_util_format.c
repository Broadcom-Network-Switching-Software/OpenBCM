/*! \file shr_util_format.c
 *
 * Utility routines for formatting output.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_util.h>

void
shr_util_format_uint32(char *buf, uint32_t n, int min_digits, int base)
{
    static char *digit_char = "0123456789abcdef";
    unsigned int tmp;
    int digit, needed_digits = 0;

    for (tmp = n, needed_digits = 0; tmp; needed_digits++) {
        tmp /= base;
    }

    if (needed_digits > min_digits) {
        min_digits = needed_digits;
    }

    buf[min_digits] = 0;

    for (digit = min_digits - 1; digit >= 0; digit--) {
        buf[digit] = digit_char[n % base];
        n /= base;
    }
}

void
shr_util_format_uint32_array(char *buf, uint32_t *val, int nval)
{
    int i = SHR_BYTES2WORDS(nval) - 1;

    /*
     * Don't skip leading 0's, as this makes packet buffers
     * and other buffer memories difficult to read and to determine
     * what the contents are.
     */

    if (i == 0 && val[i] < 10) { /* Only a single word < 10 */
        buf[0] = '0' + val[i];
        buf[1] = 0;
    } else {
        /* Print first word */
        buf[0] = '0';
        buf[1] = 'x';

        /*
         * Don't use the previous value of 1 min digit for multiple
         * integer values, as that makes a long integer of all zeroes
         * print as just '0', which makes memory contents hard to read.
         * Only use mindigit of 1 if only one integer to print
         */
        if (i == 0) {
            shr_util_format_uint32(buf + 2, val[i], 1, 16);
        } else {
            if ((nval % 4) == 0) {
                shr_util_format_uint32(buf + 2, val[i], 8, 16);
            } else {
                shr_util_format_uint32(buf + 2, val[i],
                                       (2 * (nval % 4)), 16);
            }
        }

        /* Print rest of words, if any */
        while (--i >= 0) {
            while (*buf) {
                buf++;
            }
            shr_util_format_uint32(buf, val[i], 8, 16);
        }
    }
}

void
shr_util_format_uint8_array(char *buf, const uint8_t *val, int nval)
{
    static char *digit_char = "0123456789abcdef";
    int byte;

    /* Print first word */
    buf[0] = '0';
    buf[1] = 'x';

    for (byte = nval - 1; byte >= 0; byte--) {
        buf[2 * (nval - byte)] = digit_char[(val[byte] / 16) % 16];
        buf[(2 * (nval - byte)) + 1] = digit_char[val[byte] % 16];
    }

    /* Null string terminate */
    buf[(2 * nval) + 2] = 0;
}
