/*! \file bcm56980_a0_pdma_attach.c
 *
 * Initialize PDMA driver resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef INCLUDE_PKTIO

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_cmicx.h>

#define WR_EP_INTF_CREDITS  (1U << 8)

static int
bcm56980_a0_cnet_pdma_preinit(struct pdma_hw *hw)
{
    int ep_credits;
    uint32_t val;

    /*
     * Since only 6 bits of iproc_cmic_to_ep_credits[5:0]
     * are being used, so we have to set the max credits value twice
     * in order to release 64 credits to EP.
     */
    hw->hdls.reg_rd32(hw, CMICX_EPINTF_RELEASE_CREDITS, &val);
    if (!val) {
        ep_credits = 63;
        hw->hdls.reg_wr32(hw, CMICX_EPINTF_MAX_CREDITS, WR_EP_INTF_CREDITS | ep_credits);
        hw->hdls.reg_wr32(hw, CMICX_EPINTF_RELEASE_CREDITS, 1);
        hw->hdls.reg_wr32(hw, CMICX_EPINTF_RELEASE_CREDITS, 0);
        ep_credits = 1;
        hw->hdls.reg_wr32(hw, CMICX_EPINTF_MAX_CREDITS, WR_EP_INTF_CREDITS | ep_credits);
    }

    return SHR_E_NONE;
}

int
bcm56980_a0_cnet_pdma_attach(struct pdma_dev *dev)
{
    struct pdma_hw *hw;
    int rv;

    rv = bcmcnet_cmicx_pdma_driver_attach(dev);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    hw = dev->ctrl.hw;
    hw->hdls.pre_init = bcm56980_a0_cnet_pdma_preinit;

    return SHR_E_NONE;
}

int
bcm56980_a0_cnet_pdma_detach(struct pdma_dev *dev)
{
    return bcmcnet_cmicx_pdma_driver_detach(dev);
}

#endif /* INCLUDE_PKTIO */
