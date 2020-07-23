/*! \file bcm56990_b0_feature.c
 *
 * BCM56990_B0 FEATURE driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/feature.h>
#include <bcm_int/ltsw/feature.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/******************************************************************************
 * Private functions
 */

static int
bcm56990_b0_ltsw_feature_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    ltsw_feature_enable(unit, LTSW_FT_L3);
    ltsw_feature_enable(unit, LTSW_FT_L3_FIB);
    ltsw_feature_enable(unit, LTSW_FT_RATE);
    ltsw_feature_enable(unit, LTSW_FT_MPLS);
    ltsw_feature_enable(unit, LTSW_FT_IPMC);
    ltsw_feature_enable(unit, LTSW_FT_L2_USER_ENTRY);
    ltsw_feature_enable(unit, LTSW_FT_DLB);
    ltsw_feature_enable(unit, LTSW_FT_DLB_DGM);
    ltsw_feature_enable(unit, LTSW_FT_NO_HOST);
    ltsw_feature_enable(unit, LTSW_FT_FLEXCTR_GEN2);
    ltsw_feature_enable(unit, LTSW_FT_ECN_WRED);
    ltsw_feature_enable(unit, LTSW_FT_LB_HASH);
    ltsw_feature_enable(unit, LTSW_FT_VFI);
    ltsw_feature_enable(unit, LTSW_FT_L3_FIB_SINGLE_POINTER);
    ltsw_feature_enable(unit, LTSW_FT_LDH);
    ltsw_feature_enable(unit, LTSW_FT_NO_L2_MPLS);
    ltsw_feature_enable(unit, LTSW_FT_FLEX_FLOW);
    ltsw_feature_enable(unit, LTSW_FT_L3_HIER);
    ltsw_feature_enable(unit, LTSW_FT_TNL_TERM_FLEXCTR);
    ltsw_feature_enable(unit, LTSW_FT_PAYLOAD_TPID);
    ltsw_feature_enable(unit, LTSW_FT_TIME);
    ltsw_feature_enable(unit, LTSW_FT_FLEXCTR_QUANT_32BIT);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Feature driver function variable for bcm56990_b0 device.
 */
static mbcm_ltsw_feature_drv_t bcm56990_b0_ltsw_feature_drv = {
    .feature_init = bcm56990_b0_ltsw_feature_init
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_b0_ltsw_feature_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_feature_drv_set(unit, &bcm56990_b0_ltsw_feature_drv));

exit:
    SHR_FUNC_EXIT();
}

