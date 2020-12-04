/*! \file bcmltd_bitop_range_eq.c
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
 * Same as bcmltd_bitop_range_eq, but for a single BCMLTD_BITDCL.
 */
static inline int
bcmltd_bitop_range_eq_one_bitdcl(BCMLTD_BITDCL b1,
        BCMLTD_BITDCL b2,
        int offs, int n)
{
    BCMLTD_BITDCL mask = ~0;

    mask >>= (BCMLTD_BITWID - n);
    /* Move the mask to start from 'offs' offset */
    mask <<= offs;

    return ((b1 & mask) == (b2 & mask));
}

/*!
 * \brief Test if two bit array bit ranges are equal.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref BCMLTD_BITEQ_RANGE for details.
 */
int
bcmltd_bitop_range_eq(const BCMLTD_BITDCL *a1,
        const BCMLTD_BITDCL *a2,
        int offs, int n)
{
    const BCMLTD_BITDCL *ptr1;
    const BCMLTD_BITDCL *ptr2;
    int woffs, wremain;

    if (n <= 0) {
        return 1;
    }

    ptr1 = a1 + (offs / BCMLTD_BITWID);
    ptr2 = a2 + (offs / BCMLTD_BITWID);

    woffs = offs % BCMLTD_BITWID;

    if (woffs != 0) {
        wremain = BCMLTD_BITWID - woffs;
        if (n <= wremain) {
            return bcmltd_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, woffs, n);
        }
        if (!bcmltd_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, woffs, wremain)) {
            return 0;
        }
        n -= wremain;
        ++ptr1, ++ptr2;
    }
    while (n >= BCMLTD_BITWID) {
        if (*(ptr1++) != *(ptr2++)) {
            return 0;
        }
        n -= BCMLTD_BITWID;
    }
    if (n > 0) {
        return bcmltd_bitop_range_eq_one_bitdcl(*ptr1, *ptr2, 0, n);
    }
    return 1;
}
