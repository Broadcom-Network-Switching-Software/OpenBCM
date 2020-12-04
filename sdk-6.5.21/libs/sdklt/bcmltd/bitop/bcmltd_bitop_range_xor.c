/*! \file bcmltd_bitop_range_xor.c
 *
 *  Bit array operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_bitop.h>

/*!
 * \brief Perform bitwise XOR operation on bit arrays.
 *
 * See \ref BCMLTD_BITXOR_RANGE for details.
 */
void
bcmltd_bitop_range_xor(const BCMLTD_BITDCL *a1,
        const BCMLTD_BITDCL *a2,
        const int offs, int n,
        BCMLTD_BITDCL *dest)
{
    if (n > 0) {
        BCMLTDi_BITOP_RANGE(a1, a2, offs, n, dest, ^);
    }
}
