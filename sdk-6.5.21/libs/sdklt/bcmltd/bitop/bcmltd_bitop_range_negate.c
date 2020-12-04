/*! \file bcmltd_bitop_range_negate.c
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
 * Same as bcmltd_bitop_range_negate, but for a single BCMLTD_BITDCL.
 */
static inline void
bcmltd_bitnegate_range_one_bitdcl(const BCMLTD_BITDCL *a,
        int offs, int n,
        BCMLTD_BITDCL *dest)
{
    BCMLTD_BITDCL mask = ~0;
    BCMLTD_BITDCL data;

    mask >>= (BCMLTD_BITWID - n);
    mask <<= offs;
    data =  ~*a & mask;
    *dest &= ~mask;
    *dest |= data;
}

/*!
 * \brief Negate a range of bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref BCMLTD_BITNEGATE_RANGE for details.
 */
void
bcmltd_bitop_range_negate(const BCMLTD_BITDCL *a,
        int offs, int n,
        BCMLTD_BITDCL *dest)
{
    const BCMLTD_BITDCL *pa;
    BCMLTD_BITDCL *pd;
    int woffs, wremain;

    if (n <= 0) {
        return;
    }

    pa = a + (offs / BCMLTD_BITWID);
    pd = dest + (offs / BCMLTD_BITWID);
    woffs = (offs % BCMLTD_BITWID);

    wremain = BCMLTD_BITWID - woffs;
    if (n <= wremain) {
        bcmltd_bitnegate_range_one_bitdcl(pa, woffs, n, pd);
        return;
    }
    bcmltd_bitnegate_range_one_bitdcl(pa, woffs, wremain, pd);
    n -= wremain;
    ++pa; ++pd;
    while (n >= BCMLTD_BITWID) {
        *pd = ~(*pa);
        n -= BCMLTD_BITWID;
        ++pa; ++pd;
    }
}
