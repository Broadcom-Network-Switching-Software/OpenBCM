/*! \file bcm56780_a0_ina_2_4_13_mirror.c
 *
 * BCM56780_A0 INA_2_4_13 MIRROR driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/mirror.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/xfs/mirror.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MIRROR

static const bcmint_mirror_db_t mirror_db = {
    .ifa_1_probe_encap = TRUE,
    .ifa_1_probe_encap_index = 15,
    .ifa_1_probe_encap_npl_lb_header = TRUE,
};

static int
bcm56780_a0_ina_2_4_13_ltsw_mirror_db_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_mirror_db_set(unit, &mirror_db));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief mirror driver function variable for bcm56780_a0 INA_2_4_13 device.
 */
static mbcm_ltsw_mirror_drv_t bcm56780_a0_ina_2_4_13_ltsw_mirror_drv = {
    .mirror_db_init = bcm56780_a0_ina_2_4_13_ltsw_mirror_db_init,
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_mirror_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_mirror_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_mirror_drv));

exit:
    SHR_FUNC_EXIT();
}

