/*! \file bcm56990_b0_bd_ts.c
 *
 * BCM56990 BD TimeSync driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/chip/bcm56990_b0_cmic_acc.h>
#include <bcmbd/bcmbd_cmicx_ts_internal.h>
#include <bcmbd/bcmbd_cmicx_ts_drv_internal.h>

#include "bcm56990_b0_drv.h"

/*******************************************************************************
 * Local definitions
 */

static bcmbd_cmicx_ts_drv_acc_t bcm56990_b0_ts_drv_acc = {
    .timestamp_enable_get = bcmbd_cmicx_ts_timestamp_enable_get,
    .timestamp_enable_set = bcmbd_cmicx_ts_timestamp_enable_set,
    .timestamp_get = bcmbd_cmicx_ts_timestamp_get,
    .timestamp_set = bcmbd_cmicx_ts_timestamp_set,
    .timestamp_offset_get = bcmbd_cmicx_ts_timestamp_offset_get,
    .timestamp_offset_set = bcmbd_cmicx_ts_timestamp_offset_set
};

/*******************************************************************************
 * Public function
 */

int
bcm56990_b0_bd_ts_drv_init(int unit)
{
    /* Use the CMICx TimeSync driver for BCM56990 */
    return bcmbd_cmicx_ts_drv_init(unit, &bcm56990_b0_ts_drv_acc);
}

int
bcm56990_b0_bd_ts_drv_cleanup(int unit)
{
    return bcmbd_cmicx_ts_drv_cleanup(unit);
}
