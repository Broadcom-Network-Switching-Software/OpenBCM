/*! \file bcmdrd_pbmp.c
 *
 * Utility functions used for extracting field values from
 * registers and memories.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_types.h>

/*
 * Function:
 *	bcmdrd_pbmp_is_null
 * Purpose:
 *	Check if any bits are set in a port bitmap.
 * Parameters:
 *      pbmp - port biitmap to check
 * Returns:
 *      1 if no bits are set, otherwise 0.
 */
int
bcmdrd_pbmp_is_null(const bcmdrd_pbmp_t *pbmp)
{
    int idx;

    for (idx = 0; idx < BCMDRD_PBMP_WORD_MAX; idx++) {
        if (BCMDRD_PBMP_WORD_GET(*pbmp, idx) != 0) {
            return 0;
        }
    }
    return 1;
}
