/*! \file l3.c
 *
 * BCM56880_A0 L3 Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/xfs/l3.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/******************************************************************************
 * Private functions
 */

static mbcm_ltsw_l3_drv_t bcm56880_a0_ltsw_l3_drv = {
    .l3_init = xfs_ltsw_l3_init,
    .l3_deinit = xfs_ltsw_l3_deinit,
    .l3_mtu_set = xfs_ltsw_l3_mtu_set,
    .l3_mtu_get = xfs_ltsw_l3_mtu_get,
    .l3_mtu_delete = xfs_ltsw_l3_mtu_delete,
    .l3_mtu_recover = xfs_ltsw_l3_mtu_recover,
    .l3_info_get = xfs_ltsw_l3_info_get,
    .l3_enable_set = NULL
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_l3_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_drv_set(unit, &bcm56880_a0_ltsw_l3_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_variant_drv_attach(unit, BCM56880_A0_LTSW_VARIANT_L3));
exit:
    SHR_FUNC_EXIT();
}
