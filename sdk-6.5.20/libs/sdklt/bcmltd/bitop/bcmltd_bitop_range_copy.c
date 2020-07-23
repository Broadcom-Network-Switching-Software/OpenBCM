/*! \file bcmltd_bitop_range_copy.c
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
 * Same as bcmltd_bitop_range_copy, but for a single BCMLTD_BITDCL.
 */
static inline void
bcmltd_bitop_range_copy_one_bitdcl(BCMLTD_BITDCL *a1,
        int offs1,
        BCMLTD_BITDCL a2,
        int offs2, int n)
{
    BCMLTD_BITDCL data;
    BCMLTD_BITDCL mask;
    /* Get the data */
    data = a2 >> offs2;

    /* Align the data to the place it may be inserted */
    data <<= offs1;

    /*
     * We might have bits in a2 above offs2 + n that need
     * to be cleared.
     */
    mask = ~0;
    mask >>= BCMLTD_BITWID - n;
    mask <<= offs1;
    data &= mask;
    *a1 &= ~mask;
    *a1 |= data;
}

/*!
 * \brief Copy a range of bits between two bit arrays.
 *
 * INTERNAL USE ONLY.
 *
 * Refer to \ref BCMLTD_BITCOPY_RANGE macro.
 */
void
bcmltd_bitop_range_copy(BCMLTD_BITDCL *a1,
        int offs1,
        const BCMLTD_BITDCL *a2,
        int offs2, int n)
{
    BCMLTD_BITDCL *cur_dst;
    const BCMLTD_BITDCL *cur_src;
    int woff_src, woff_dst, wremain;

    if (n <= 0) {
        return;
    }

    if ((offs1 % BCMLTD_BITWID) == 0 &&
        (offs2 % BCMLTD_BITWID) == 0 &&
        (n % BCMLTD_BITWID) == 0) {
        /* Both source, destination and size are BCMLTD_BITDCL aligned */
        sal_memcpy(&(a1[offs1 / BCMLTD_BITWID]), &(a2[offs2 / BCMLTD_BITWID]),
                   BCMLTD_BIT_ALLOCSIZE(n));
        return;
    }

    cur_dst = a1 + (offs1 / BCMLTD_BITWID);
    cur_src = a2 + (offs2 / BCMLTD_BITWID);

    woff_src = offs2 % BCMLTD_BITWID;
    woff_dst = offs1 % BCMLTD_BITWID;

    if (woff_dst >= woff_src) {
        wremain = BCMLTD_BITWID - woff_dst;
    } else {
        wremain = BCMLTD_BITWID - woff_src;
    }
    if (n <= wremain) {
        bcmltd_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                        *cur_src, woff_src, n);
        return;
    }
    bcmltd_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                    *cur_src, woff_src, wremain);
    n -= wremain;
    while (n >= BCMLTD_BITWID) {
        if (woff_dst >= woff_src) {
            ++cur_dst;
            wremain = woff_dst - woff_src;
            if (wremain > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                BCMLTD_BITWID - wremain, wremain);
            }
        } else {
            ++cur_src;
            wremain = woff_src - woff_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, wremain);
        }
        n -= wremain;
        wremain = BCMLTD_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, wremain);
        } else {
            ++cur_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, wremain);
        }
        n -= wremain;
    }

    if (woff_dst >= woff_src) {
        ++cur_dst;
        wremain = woff_dst - woff_src;
        if (n <= wremain) {
            if (n > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                BCMLTD_BITWID - wremain, n);
            }
            return;
        }
        if (wremain > 0) {
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, wremain);
        }
    } else {
        ++cur_src;
        wremain = woff_src - woff_dst;
        if (n <= wremain) {
            if (n > 0) {
                bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                                *cur_src, 0, n);
            }
            return;
        }
        bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                        *cur_src, 0, wremain);
    }
    n -= wremain;

    if (n > 0) {
        wremain = BCMLTD_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, BCMLTD_BITWID - wremain,
                                            *cur_src, 0, n);
        } else {
            ++cur_dst;
            bcmltd_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            BCMLTD_BITWID - wremain, n);
        }
    }
}
