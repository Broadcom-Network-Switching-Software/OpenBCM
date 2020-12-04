/*! \file shr_bitop_range_null.c
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
 * Same as shr_bitop_range_null, but for a single SHR_BITDCL.
 */
static inline int
shr_bitop_range_null_one_bitdcl(SHR_BITDCL b, int offs, int n)
{
    SHR_BITDCL mask = ~0;

    mask >>= (SHR_BITWID - n);
    /* Move the mask to start from 'offs' offset */
    mask <<= offs;

    return ((b & mask) == 0);
}

/*!
 * \brief Test if bits are cleared in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref SHR_BITNULL_RANGE for details.
 */
int
shr_bitop_range_null(const SHR_BITDCL *a, int offs, int n)
{
    const SHR_BITDCL *ptr;
    int woffs, wremain;

    if (n <= 0) {
        return 1;
    }

    /* Pointer to offs SHR_BITDCL in 'a' that contains 'offs' */
    ptr = a + (offs / SHR_BITWID);

    /* Offset of 'offs' bit within this SDH_BITDCL */
    woffs = offs % SHR_BITWID;

    /* Check if 'offs' is SHR_BITWID aligned */
    if (woffs != 0) {
        /*  Get remaining bits in this SDH_BITDCL */
        wremain = SHR_BITWID - woffs;
        if (n <= wremain) {
            /* All the range is in one SHR_BITDCL */
            return shr_bitop_range_null_one_bitdcl(*ptr,
                                                   woffs,
                                                   n);
        }
        /* We should check the offs SHR_BITDCL, and might also continue */
        if (!shr_bitop_range_null_one_bitdcl(*ptr, woffs, wremain)) {
            return 0;
        }
        n -= wremain;
        ++ptr;
    }
    while (n >= SHR_BITWID) {
        /* We're testing a full SHR_BITDCL */
        if (*(ptr++)) {
            return 0;
        }
        n -= SHR_BITWID;
    }
    /* This is the last SHR_BITDCL, and it is not SHR_BITWID */
    if (n > 0) {
        return shr_bitop_range_null_one_bitdcl(*ptr, 0, n);
    }
    return 1;
}
