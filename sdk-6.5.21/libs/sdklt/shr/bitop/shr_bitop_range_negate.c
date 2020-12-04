/*! \file shr_bitop_range_negate.c
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
 * Same as shr_bitop_range_negate, but for a single SHR_BITDCL.
 */
static inline void
shr_bitnegate_range_one_bitdcl(const SHR_BITDCL *a,
                               int offs, int n, SHR_BITDCL *dest)
{
    SHR_BITDCL mask = ~0;
    SHR_BITDCL data;

    mask >>= (SHR_BITWID - n);
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
 * See \ref SHR_BITNEGATE_RANGE for details.
 */
void
shr_bitop_range_negate(const SHR_BITDCL *a,
                       int offs, int n, SHR_BITDCL *dest)
{
    const SHR_BITDCL *pa;
    SHR_BITDCL *pd;
    int woffs, wremain;

    if (n <= 0) {
        return;
    }

    pa = a + (offs / SHR_BITWID);
    pd = dest + (offs / SHR_BITWID);
    woffs = (offs % SHR_BITWID);

    wremain = SHR_BITWID - woffs;
    if (n <= wremain) {
        shr_bitnegate_range_one_bitdcl(pa, woffs, n, pd);
        return;
    }
    shr_bitnegate_range_one_bitdcl(pa, woffs, wremain, pd);
    n -= wremain;
    ++pa; ++pd;
    while (n >= SHR_BITWID) {
        *pd = ~(*pa);
        n -= SHR_BITWID;
        ++pa; ++pd;
    }
    if (n > 0) {
        shr_bitnegate_range_one_bitdcl(pa, 0, n, pd);
    }
}
