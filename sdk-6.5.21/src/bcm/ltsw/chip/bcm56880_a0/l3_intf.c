/*! \file l3_intf.c
 *
 * BCM56880_A0 L3 Intf Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

#include <shr/shr_debug.h>


/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_l3_intf_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_ltsw_variant_drv_attach(unit,
                                             BCM56880_A0_LTSW_VARIANT_L3_INTF));
exit:
    SHR_FUNC_EXIT();
}
