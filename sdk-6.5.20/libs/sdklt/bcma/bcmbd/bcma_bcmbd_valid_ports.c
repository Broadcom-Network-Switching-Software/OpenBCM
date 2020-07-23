/*! \file bcma_bcmbd_valid_ports.c
 *
 * Retrieve valid port bitmaps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_symbol_block_valid_ports_get(int unit, int enum_val, int blknum,
                                        bcmdrd_pbmp_t *pbmp)
{
    int idx;
    bcmdrd_port_num_domain_t pnd;
    const bcmdrd_block_t *blkp = NULL;
    const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(unit);

    if (cinfo == NULL) {
        return -1;
    }

    for (idx = 0; idx < cinfo->nblocks; idx++) {
        if (cinfo->blocks[idx].blknum == blknum) {
            blkp = cinfo->blocks + idx;
            break;
        }
    }
    if (blkp == NULL) {
        return -1;
    }
    pnd = bcmdrd_chip_port_num_domain(cinfo, enum_val, blkp->type);
    bcmdrd_chip_valid_pbmp(cinfo, pnd, pbmp);
    if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type)) {
        BCMDRD_PBMP_AND(*pbmp, blkp->pbmps);
    }
    return 0;
}
