/*! \file bcmltd_bitop_range_count.c
 *
 * Bit array operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_bitop.h>

/*!
 * INTERNAL USE ONLY.
 *
 * BCMLTD_BITDCL bit set count
 */
static inline int
bcmltd_popcount(BCMLTD_BITDCL w)
{
    w = w - ((w >> 1) & 0x5555555555555555ULL);
    w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
    return ((((w + (w >> 4)) & 0xF0F0F0F0F0F0F0FULL) * 0x101010101010101ULL) >> 56);
}

/*!
 * INTERNAL USE ONLY.
 *
 * Same as bcmltd_bitop_range_count, but for a single BCMLTD_BITDCL.
 */
static inline int
bcmltd_bitop_range_count_one_bitdcl(BCMLTD_BITDCL b, int offs, int n)
{
    BCMLTD_BITDCL mask;
    BCMLTD_BITDCL count = 0;
    unsigned int i = 0;

    if (n >= 0) {
        mask = ~0;
        mask >>= (BCMLTD_BITWID - n);
        mask <<= offs;
        b &= mask;
    }

    do {
        count += bcmltd_popcount(b >> (i * sizeof(BCMLTD_BITDCL) * 8));
    } while (++i < sizeof(BCMLTD_BITDCL) / sizeof(BCMLTD_BITDCL));

    return count;
}

/*!
 * \brief Count the number of bits set in a range of bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref BCMLTD_BITCOUNT_RANGE for details.
 */
void
bcmltd_bitop_range_count(const BCMLTD_BITDCL *bits, int offs, int n, int *count)
{
    const BCMLTD_BITDCL *ptr;
    int woffs, wremain;

    *count = 0;

    if (n <= 0) {
        return;
    }

    ptr = bits + (offs / BCMLTD_BITWID);
    woffs = offs % BCMLTD_BITWID;

    if (woffs != 0) {
        wremain = BCMLTD_BITWID - woffs;
        if (n <= wremain) {
            *count = bcmltd_bitop_range_count_one_bitdcl(*ptr, woffs, n);
            return;
        }
        *count += bcmltd_bitop_range_count_one_bitdcl(*ptr, woffs, wremain);
        n -= wremain;
        ++ptr;
    }
    while (n >= BCMLTD_BITWID) {
        *count += bcmltd_bitop_range_count_one_bitdcl(*(ptr++), 0, -1);
        n -= BCMLTD_BITWID;
    }
    if (n > 0) {
        *count += bcmltd_bitop_range_count_one_bitdcl(*ptr, 0, n);
    }
}
