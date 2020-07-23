/*! \file shr_bitop_range_count.c
 *
 * Bit array operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>

static inline int
shr_popcount32(uint32_t w)
{
    w = (w & 0x55555555) + ((w >> 1) & 0x55555555);
    w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    w = (w + (w >> 4)) & 0x0f0f0f0f;
    w = w + (w >> 8);

    return (w + (w >> 16)) & 0xff;
}

/*!
 * INTERNAL USE ONLY.
 *
 * Same as shr_bitop_range_count, but for a single SHR_BITDCL.
 */
static inline int
shr_bitop_range_count_one_bitdcl(SHR_BITDCL b, int offs, int n)
{
    SHR_BITDCL mask;
    int count = 0;
    unsigned int i = 0;

    if (n >= 0) {
        mask = ~0;
        mask >>= (SHR_BITWID - n);
        mask <<= offs;
        b &= mask;
    }

    do {
        count += shr_popcount32(b >> (i * sizeof(uint32_t) * 8));
    } while (++i < sizeof(SHR_BITDCL) / sizeof(uint32_t));

    return count;
}

/*!
 * \brief Count the number of bits set in a range of bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITCOUNT_RANGE for details.
 */
void
shr_bitop_range_count(const SHR_BITDCL *bits, int offs, int n, int *count)
{
    const SHR_BITDCL *ptr;
    int woffs, wremain;

    *count = 0;

    if (n <= 0) {
        return;
    }

    ptr = bits + (offs / SHR_BITWID);
    woffs = offs % SHR_BITWID;

    if (woffs != 0) {
        wremain = SHR_BITWID - woffs;
        if (n <= wremain) {
            *count = shr_bitop_range_count_one_bitdcl(*ptr, woffs, n);
            return;
        }
        *count += shr_bitop_range_count_one_bitdcl(*ptr, woffs, wremain);
        n -= wremain;
        ++ptr;
    }
    while (n >= SHR_BITWID) {
        *count += shr_bitop_range_count_one_bitdcl(*(ptr++), 0, -1);
        n -= SHR_BITWID;
    }
    if (n > 0) {
        *count += shr_bitop_range_count_one_bitdcl(*ptr, 0, n);
    }
}
