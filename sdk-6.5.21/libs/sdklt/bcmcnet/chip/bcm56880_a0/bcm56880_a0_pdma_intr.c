/*! \file bcm56880_a0_pdma_intr.c
 *
 * Initialize PDMA driver resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmcnet/bcmcnet_core.h>

int
bcm56880_a0_cnet_pdma_intr_enable(int unit, int cmc, int inum)
{
    bcmbd_cmicx_intr_enable(unit, inum);
    return 0;
}

int
bcm56880_a0_cnet_pdma_intr_disable(int unit, int cmc, int inum)
{
    bcmbd_cmicx_intr_disable_deferred(unit, inum);
    return 0;
}

int
bcm56880_a0_cnet_pdma_intr_connect(int unit, int cmc, int inum,
                                   bcmcnet_intr_f ifunc, uint32_t idata)
{
    return bcmbd_cmicx_intr_func_set(unit, inum, ifunc, idata);
}

