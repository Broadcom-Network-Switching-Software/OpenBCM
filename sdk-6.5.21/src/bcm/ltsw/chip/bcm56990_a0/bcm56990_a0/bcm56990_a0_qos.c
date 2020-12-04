/*! \file bcm56990_a0_qos.c
 *
 * BCM56990_A0 QoS subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/qos.h>
#include <bcm_int/ltsw/xgs/qos.h>
#include <bcm_int/ltsw/qos_int.h>
#include <bcm_int/ltsw/dev.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_QOS

/******************************************************************************
 * Private functions
 */

static const bcmint_qos_lt_t bcm56990_a0_qos_lt[] = {{0}};

static int
bcm56990_a0_ltsw_qos_lt_db_get(int unit, bcmint_qos_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_MEMORY);

    lt_db->lt_bmp = 0;

    lt_db->lts = bcm56990_a0_qos_lt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief QoS sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_qos_drv_t bcm56990_a0_qos_sub_drv = {
    .qos_lt_db_get = bcm56990_a0_ltsw_qos_lt_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_qos_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_qos_drv_set(unit, &bcm56990_a0_qos_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
