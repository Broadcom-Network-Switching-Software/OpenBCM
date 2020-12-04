/*! \file bcm56780_a0_pdma_attach.c
 *
 * Initialize PDMA driver resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_cmicx.h>

int
bcm56780_a0_cnet_pdma_attach(struct pdma_dev *dev)
{
    return bcmcnet_cmicx_pdma_driver_attach(dev);
}

int
bcm56780_a0_cnet_pdma_detach(struct pdma_dev *dev)
{
    return bcmcnet_cmicx_pdma_driver_detach(dev);
}

