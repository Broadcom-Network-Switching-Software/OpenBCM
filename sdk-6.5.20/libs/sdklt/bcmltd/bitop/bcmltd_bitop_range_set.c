/*! \file bcmltd_bitop_range_set.c
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
 * Same as bcmltd_bitop_range_set, but for a single BCMLTD_BITDCL.
 */
static inline void
bcmltd_bitop_range_set_one_bitdcl(BCMLTD_BITDCL *a, int offs, int n)
{
    BCMLTD_BITDCL mask = ~0;
    mask >>= (BCMLTD_BITWID - n);
    mask <<= offs;
    *a |= mask;
}

/*!
 * \brief Set range of bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * Refer to \ref BCMLTD_BITCLR_RANGE macro.
 */
void
bcmltd_bitop_range_set(BCMLTD_BITDCL *a, int offs, int n)
{
    BCMLTD_BITDCL *pa;
    int woffs, wremain;

    if (n <= 0) {
        return;
    }

    pa = a + (offs / BCMLTD_BITWID);

    woffs = offs % BCMLTD_BITWID;

    if (woffs != 0) {
        wremain = BCMLTD_BITWID - woffs;
        if (n <= wremain) {
            bcmltd_bitop_range_set_one_bitdcl(pa, woffs, n);
            return;
        }
        bcmltd_bitop_range_set_one_bitdcl(pa, woffs, wremain);
        n -= wremain;
        ++pa;
    }
    while (n >= BCMLTD_BITWID) {
        *(pa++) = ~0;
        n -= BCMLTD_BITWID;
    }
    if (n > 0) {
        bcmltd_bitop_range_set_one_bitdcl(pa, 0, n);
    }
}
