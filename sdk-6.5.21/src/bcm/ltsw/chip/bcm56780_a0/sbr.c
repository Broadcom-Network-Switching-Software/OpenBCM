/*
 * Strength based resolution (SBR) driver for BCM56780 A0.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ltsw_sbr_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ltsw_variant_drv_attach(unit,
                                             BCM56780_A0_LTSW_VARIANT_SBR));

exit:
    SHR_FUNC_EXIT();
}

