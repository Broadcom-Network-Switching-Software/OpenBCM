/*! \file bcm56990_a0_bd_m0ssq.c
 *
 * bcm56990_a0 device-specific M0SSQ driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>

#include "bcm56990_a0_drv.h"

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* Base address of M0SSQ block. */
#define M0SSQ_BASE_ADDR           0x1300000

/*******************************************************************************
 * Local data
 */

/* Device-specific M0SSQ driver. */
static bcmbd_m0ssq_drv_t bcm56990_a0_m0ssq_drv;

/*******************************************************************************
 * M0SSQ CMICX M0SSQ Driver
 */
static int
bcm56990_a0_m0ssq_init(int unit)
{
    bcmbd_m0ssq_dev_t *dev;
    const bcmbd_m0ssq_drv_t *base_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    /* Setup chip-specific data. */
    dev->base_addr = M0SSQ_BASE_ADDR;

    /* Create mem objects data from template. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_m0ssq_mem_create(unit));

    /* Invoke init function of base driver. */
    base_drv = bcmbd_cmicx2_m0ssq_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (base_drv->init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions.
 */
int
bcm56990_a0_bd_m0ssq_drv_init(int unit)
{
    const bcmbd_m0ssq_drv_t *base_drv;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    /* Get base driver. */
    base_drv = bcmbd_cmicx2_m0ssq_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);

    /* Inherit driver methods from M0SSQ base driver. */
    sal_memcpy(&bcm56990_a0_m0ssq_drv, base_drv, sizeof(bcmbd_m0ssq_drv_t));

    /* Override chip-specific method. */
    bcm56990_a0_m0ssq_drv.init = bcm56990_a0_m0ssq_init;

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_drv_init(unit, &bcm56990_a0_m0ssq_drv));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_bd_m0ssq_drv_cleanup(int unit)
{
    return bcmbd_m0ssq_drv_init(unit, NULL);
}
