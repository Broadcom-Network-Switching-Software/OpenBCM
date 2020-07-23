/*! \file lb_hash.c
 *
 * LB hash management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/lb_hash.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Lb_hash driver function variable for bcm56780_a0 device.
 */
static mbcm_ltsw_lb_hash_drv_t bcm56780_a0_ltsw_lb_hash_drv = {
    NULL
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ltsw_lb_hash_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv_set(unit, &bcm56780_a0_ltsw_lb_hash_drv));

exit:
    SHR_FUNC_EXIT();
}

