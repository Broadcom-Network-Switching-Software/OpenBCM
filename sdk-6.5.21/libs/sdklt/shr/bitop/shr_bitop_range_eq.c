/*! \file shr_bitop_range_eq.c
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
 * Same as shr_bitop_range_eq, but for a single SHR_BITDCL.
 */
static inline int
shr_bitop_range_eq_one_bitdcl(SHR_BITDCL b1, SHR_BITDCL b2, int offs, int n)
{
    SHR_BITDCL mask = ~0;

    mask >>= (SHR_BITWID - n);
    /* Move the mask to start from 'offs' offset */
    mask <<= offs;

    return ((b1 & mask) == (b2 & mask));
}

/*!
 * \brief Test if two bit array bit ranges are equal.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITEQ_RANGE for details.
 */
int
shr_bitop_range_eq(const SHR_BITDCL *a1, const SHR_BITDCL *a2,
                   int offs, int n)
{
    const SHR_BITDCL *ptr1;
    const SHR_BITDCL *ptr2;
    int woffs, wremain;

    if (n <= 0) {
        return 1;
    }

    ptr1 = a1 + (offs / SHR_BITWID);
    ptr2 = a2 + (offs / SHR_BITWID);

    woffs = offs % SHR_BITWID;

    if (woffs != 0) {
        wremain = SHR_BITWID - woffs;
        if (n <= wremain) {
            return shr_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, woffs, n);
        }
        if (!shr_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, woffs, wremain)) {
            return 0;
        }
        n -= wremain;
        ++ptr1, ++ptr2;
    }
    while (n >= SHR_BITWID) {
        if (*(ptr1++) != *(ptr2++)) {
            return 0;
        }
        n -= SHR_BITWID;
    }
    if (n > 0) {
        return shr_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, 0, n);
    }
    return 1;
}
