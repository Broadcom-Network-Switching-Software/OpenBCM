/*! \file shr_pb_format_uint64.c
 *
 * Format uint64_t value(s) to print buffer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_pb_format.h>

const char *
shr_pb_format_uint64(shr_pb_t *pb, const char *prefix,
                     const uint64_t *val, int count, uint32_t bitwidth)
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
         * for the most significant uint64 to support leading zeros format.
         */
        minwidth = ((bitwidth + 3) / 4) % 16;
        if (minwidth == 0) {
            minwidth = 16;
        }
        start = (bitwidth - 1) / 64;

        /* Check the bit width of the given data array. */
        for (idx = count - 1; idx >= 0; idx--) {
            if (val[idx] == 0) {
                continue;
            }
            if (datawidth == 0) {
                for (bit = 63; bit >= 0; bit--) {
                    if (val[idx] >> bit) {
                        datawidth = bit + 1;
                        break;
                    }
                }
            } else {
                datawidth += 64;
            }
        }

        /* Ignore leading zeros format if data exceeds the format width. */
        if (datawidth > bitwidth) {
            minwidth = 0;
        }
    }

    if (minwidth > 0) {
        uint64_t val64;

        /*
         * Left-justify with the leading zeros to
         * match the specified bit width.
         */
        for (idx = start; idx >= 0; idx--) {
            if (idx >= count) {
                val64 = 0;
            } else {
                val64 = val[idx];
            }
            if (idx == start) {
                if (minwidth > 8) {
                    shr_pb_printf(pb, "0x%.*"PRIx64, minwidth - 8, val64 >> 32);
                    shr_pb_printf(pb, "_%08"PRIx64, val64 & 0xffffffff);
                } else {
                    shr_pb_printf(pb, "0x%.*"PRIx64, minwidth, val64);
                }
            } else {
                shr_pb_printf(pb, "_%08"PRIx64, val64 >> 32);
                shr_pb_printf(pb, "_%08"PRIx64, val64 & 0xffffffff);
            }
        }
    } else {
        /*
         * Format values without leading zeros.
         */
        for (start = -1, idx = count - 1; idx >= 0; idx--) {
            if (start < 0) {
                if (idx == 0 && val[idx] < 10) {
                    shr_pb_printf(pb, "%"PRIu64, val[idx]);
                    return shr_pb_str(pb);
                }
                /* Skip the leading zero elements. */
                if (val[idx] == 0) {
                    continue;
                }
                /* format the first non-zero element. */
                if (val[idx] > 0xffffffff) {
                    shr_pb_printf(pb, "0x%"PRIx64, val[idx] >> 32);
                    shr_pb_printf(pb, "_%08"PRIx64, val[idx] & 0xffffffff);
                } else {
                    shr_pb_printf(pb, "0x%"PRIx64, val[idx]);
                }
                start = idx;
                continue;
            }
            shr_pb_printf(pb, "_%08"PRIx64, val[idx] >> 32);
            shr_pb_printf(pb, "_%08"PRIx64, val[idx] & 0xffffffff);
        }
    }

    return shr_pb_str(pb);
}
