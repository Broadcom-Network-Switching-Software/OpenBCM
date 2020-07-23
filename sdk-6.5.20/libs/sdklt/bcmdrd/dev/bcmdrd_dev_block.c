/*! \file bcmdrd_dev_block.c
 *
 * Device block related functions in DRD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

int
bcmdrd_dev_block_number(int unit, int blktype, int blkinst)
{
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_block_number(cinfo, blktype, blkinst);
}
