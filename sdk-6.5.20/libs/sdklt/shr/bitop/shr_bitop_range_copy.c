/*! \file shr_bitop_range_copy.c
 *
 * Bit array operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>

/*!
 * INTERNAL USE ONLY.
 *
 * Same as shr_bitop_range_copy, but for a single SHR_BITDCL.
 */
static inline void
shr_bitop_range_copy_one_bitdcl(SHR_BITDCL *a1, int offs1, SHR_BITDCL a2,
                                int offs2, int n)
{
    SHR_BITDCL data;
    SHR_BITDCL mask;

    /*
     * No need to check that offs1 == 0 and offs2 == 0, It
     * must be becuse of the constraints.
     */
    if (n == SHR_BITWID) {
        *(a1) = a2;
        return;
    }

    /* Get the data */
    data = a2 >> offs2;

    /* Align the data to the place it may be inserted */
    data <<= offs1;

    /*
     * We might have bits in a2 above offs2 + n that need
     * to be cleared.
     */
    mask = ~0;
    mask >>= SHR_BITWID - n;
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
 * Refer to \ref SHR_BITCOPY_RANGE macro.
 */
void
shr_bitop_range_copy(SHR_BITDCL *a1, int offs1, const SHR_BITDCL *a2,
                     int offs2, int n)
{
    SHR_BITDCL *cur_dst;
    const SHR_BITDCL *cur_src;
    int woff_src, woff_dst, wremain;

    if (n <= 0) {
        return;
    }

    if ((offs1 % SHR_BITWID) == 0 &&
        (offs2 % SHR_BITWID) == 0 &&
        (n % SHR_BITWID) == 0) {
        /* Both source, destination and size are SHR_BITDCL aligned */
        sal_memcpy(&(a1[offs1 / SHR_BITWID]), &(a2[offs2 / SHR_BITWID]),
                   SHR_BITALLOCSIZE(n));
        return;
    }

    cur_dst = a1 + (offs1 / SHR_BITWID);
    cur_src = a2 + (offs2 / SHR_BITWID);

    woff_src = offs2 % SHR_BITWID;
    woff_dst = offs1 % SHR_BITWID;

    if (woff_dst >= woff_src) {
        wremain = SHR_BITWID - woff_dst;
    } else {
        wremain = SHR_BITWID - woff_src;
    }
    if (n <= wremain) {
        shr_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                        *cur_src, woff_src, n);
        return;
    }
    shr_bitop_range_copy_one_bitdcl(cur_dst, woff_dst,
                                    *cur_src, woff_src, wremain);
    n -= wremain;
    while (n >= SHR_BITWID) {
        if (woff_dst >= woff_src) {
            ++cur_dst;
            wremain = woff_dst - woff_src;
            if (wremain > 0) {
                shr_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                SHR_BITWID - wremain, wremain);
            }
        } else {
            ++cur_src;
            wremain = woff_src - woff_dst;
            shr_bitop_range_copy_one_bitdcl(cur_dst, SHR_BITWID - wremain,
                                            *cur_src, 0, wremain);
        }
        n -= wremain;
        wremain = SHR_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            shr_bitop_range_copy_one_bitdcl(cur_dst, SHR_BITWID - wremain,
                                            *cur_src, 0, wremain);
        } else {
            ++cur_dst;
            shr_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            SHR_BITWID - wremain, wremain);
        }
        n -= wremain;
    }

    if (woff_dst >= woff_src) {
        ++cur_dst;
        wremain = woff_dst - woff_src;
        if (n <= wremain) {
            if (n > 0) {
                shr_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                                SHR_BITWID - wremain, n);
            }
            return;
        }
        if (wremain > 0) {
            shr_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            SHR_BITWID - wremain, wremain);
        }
    } else {
        ++cur_src;
        wremain = woff_src - woff_dst;
        if (n <= wremain) {
            if (n > 0) {
                shr_bitop_range_copy_one_bitdcl(cur_dst, SHR_BITWID - wremain,
                                                *cur_src, 0, n);
            }
            return;
        }
        shr_bitop_range_copy_one_bitdcl(cur_dst, SHR_BITWID - wremain,
                                        *cur_src, 0, wremain);
    }
    n -= wremain;

    if (n > 0) {
        wremain = SHR_BITWID - wremain;
        if (woff_dst >= woff_src) {
            ++cur_src;
            shr_bitop_range_copy_one_bitdcl(cur_dst, SHR_BITWID - wremain,
                                            *cur_src, 0, n);
        } else {
            ++cur_dst;
            shr_bitop_range_copy_one_bitdcl(cur_dst, 0, *cur_src,
                                            SHR_BITWID - wremain, n);
        }
    }
}
