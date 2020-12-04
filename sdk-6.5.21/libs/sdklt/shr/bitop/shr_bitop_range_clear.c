/*! \file shr_bitop_range_clear.c
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
 * Same as shr_bitop_range_clear, but for a single SHR_BITDCL.
 */
static inline void
shr_bitop_range_clear_one_bitdcl(SHR_BITDCL *a, int offs, int n)
{
    SHR_BITDCL mask = ~0;

    mask >>= (SHR_BITWID - n);
    mask <<= offs;
    *a &= ~mask;
}

/*!
 * \brief Clear range of bits in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * Refer to \ref SHR_BITCLR_RANGE macro.
 */
void
shr_bitop_range_clear(SHR_BITDCL *a, int offs, int n)
{
    SHR_BITDCL *pa;
    int woffs, wremain;

    if (n <= 0) {
        return;
    }

    pa = a + (offs / SHR_BITWID);

    woffs = offs % SHR_BITWID;

    if (woffs != 0) {
        wremain = SHR_BITWID - woffs;
        if (n <= wremain) {
            shr_bitop_range_clear_one_bitdcl(pa, woffs, n);
            return;
        }
        shr_bitop_range_clear_one_bitdcl(pa, woffs, wremain);
        n -= wremain;
        ++pa;
    }
    while (n >= SHR_BITWID) {
        *(pa++) = 0;
        n -= SHR_BITWID;
    }

    if (n > 0) {
        shr_bitop_range_clear_one_bitdcl(pa, 0, n);
    }
}
