/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/ltsw/mbcm/ifa.h>
#include <bcm_int/ltsw/xfs/ifa.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ltsw_ifa_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ltsw_variant_drv_attach(unit,
                                             BCM56780_A0_LTSW_VARIANT_IFA));

exit:
    SHR_FUNC_EXIT();
}

