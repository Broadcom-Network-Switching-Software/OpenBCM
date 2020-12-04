/*! \file shr_pb_format_uint32.c
 *
 * Format uint32_t value(s) to print buffer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_pb_format.h>

const char *
shr_pb_format_uint32(shr_pb_t *pb, const char *prefix,
                     const uint32_t *val, int count, uint32_t bitwidth)
{
    int idx, bit;
    int start = 0, minwidth = 0;
    uint32_t datawidth = 0;

    if (pb == NULL || count <= 0) {
        return SHR_PB_ERR_STR;
    }

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
    }

    if (bitwidth > 0) {
        /*
         * If the format bit width is specified, get the number of hex-digits
         * for the most significant word to support leading zeros format.
         */
        minwidth = ((bitwidth + 3) / 4) % 8;
        if (minwidth == 0) {
            minwidth = 8;
        }
        start = (bitwidth - 1) / 32;

        /* Check the data bit width of the given array. */
        for (idx = count - 1; idx >= 0; idx--) {
            if (val[idx] == 0) {
                continue;
            }
            if (datawidth == 0) {
                for (bit = 31; bit >= 0; bit--) {
                    if (val[idx] >> bit) {
                        datawidth = bit + 1;
                        break;
                    }
                }
            } else {
                datawidth += 32;
            }
        }

        /* Ignore leading zeros format if data exceeds the format width. */
        if (datawidth > bitwidth) {
            minwidth = 0;
        }
    }

    if (minwidth > 0) {
        uint32_t val32;

        /*
         * Left-justify with the leading zeros to
         * match the specified bit width.
         */
        for (idx = start; idx >= 0; idx--) {
            if (idx >= count) {
                val32 = 0;
            } else {
                val32 = val[idx];
            }
            if (idx == start) {
                shr_pb_printf(pb, "0x%.*"PRIx32, minwidth, val32);
            } else {
                shr_pb_printf(pb, "_%08"PRIx32, val32);
            }
        }
    } else {
        /*
         * Format values without leading zeros.
         */
        for (start = -1, idx = count - 1; idx >= 0; idx--) {
            if (start < 0) {
                if (idx == 0 && val[idx] < 10) {
                    shr_pb_printf(pb, "%"PRIu32, *val);
                    return shr_pb_str(pb);
                }
                /* Skip the leading zero elements. */
                if (val[idx] == 0) {
                    continue;
                }
                /* format the first non-zero element. */
                shr_pb_printf(pb, "0x%"PRIx32, val[idx]);
                start = idx;
                continue;
            }
            shr_pb_printf(pb, "_%08"PRIx32, val[idx]);
        }
    }

    return shr_pb_str(pb);
}
