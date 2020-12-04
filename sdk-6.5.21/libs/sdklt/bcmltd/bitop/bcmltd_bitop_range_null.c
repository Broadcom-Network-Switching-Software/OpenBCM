/*! \file bcmltd_bitop_range_null.c
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
 * Same as bcmltd_bitop_range_null, but for a single BCMLTD_BITDCL.
 */
static inline int
bcmltd_bitop_range_null_one_bitdcl(BCMLTD_BITDCL b, int offs, int n)
{
    BCMLTD_BITDCL mask = ~0;

    mask >>= (BCMLTD_BITWID - n);
    /* Move the mask to start from 'offs' offset */
    mask <<= offs;

    return ((b & mask) == 0);
}

/*!
 * \brief Test if bits are cleared in a bit array.
 *
 * INTERNAL USE ONLY.
 *
 * See \ref BCMLTD_BITNULL_RANGE for details.
 */
int
bcmltd_bitop_range_null(const BCMLTD_BITDCL *a, int offs, int n)
{
    const BCMLTD_BITDCL *ptr;
    int woffs, wremain;

    if (n <= 0) {
        return 1;
    }

    /* Pointer to offs BCMLTD_BITDCL in 'a' that contains 'offs' */
    ptr = a + (offs / BCMLTD_BITWID);

    /* Offset of 'offs' bit within this BCMLTD_BITDCL */
    woffs = offs % BCMLTD_BITWID;

    /* Check if 'offs' is BCMLTD_BITWID aligned */
    if (woffs != 0) {
        /*  Get remaining bits in this BCMLTD_BITDCL */
        wremain = BCMLTD_BITWID - woffs;
        if (n <= wremain) {
            /* All the range is in one BCMLTD_BITDCL */
            return bcmltd_bitop_range_null_one_bitdcl(*ptr,
                                                   woffs,
                                                   n);
        }
        /* We should check the offs BCMLTD_BITDCL, and might also continue */
        if (!bcmltd_bitop_range_null_one_bitdcl(*ptr, woffs, wremain)) {
            return 0;
        }
        n -= wremain;
        ++ptr;
    }
    while (n >= BCMLTD_BITWID) {
        /* We're testing a full BCMLTD_BITDCL */
        if (*(ptr++)) {
            return 0;
        }
        n -= BCMLTD_BITWID;
    }
    /* This is the last BCMLTD_BITDCL, and it is not BCMLTD_BITWID */
    if (n > 0) {
        return bcmltd_bitop_range_null_one_bitdcl(*ptr, 0, n);
    }
    return 1;
}
