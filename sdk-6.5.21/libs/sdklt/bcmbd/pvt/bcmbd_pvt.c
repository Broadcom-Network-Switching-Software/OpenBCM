/*! \file bcmbd_pvt.c
 *
 * PVT utility driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_pvt.h>
#include <bcmbd/bcmbd_pvt_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* PVT driver for each unit. */
static bcmbd_pvt_drv_t *bd_pvt_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_pvt_drv_init(int unit, bcmbd_pvt_drv_t *drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_pvt_drv[unit] = drv;

    return SHR_E_NONE;
}


int
bcmbd_pvt_event_func_set(int unit, bcmbd_pvt_event_cb_f func)
{
    bcmbd_pvt_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_pvt_drv[unit];
    if (drv && drv->event_func_set) {
        return drv->event_func_set(unit, func);
    }

    return SHR_E_UNAVAIL;
}


int
bcmbd_pvt_temperature_get(int unit,
                          size_t size,
                          bcmbd_pvt_equation_t equation,
                          bcmbd_pvt_temp_rec_t *readout,
                          size_t *num_rec)
{
    bcmbd_pvt_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_pvt_drv[unit];
    if (drv && drv->temp_get) {
        return drv->temp_get(unit, size, equation, readout, num_rec);
    }

    return SHR_E_UNAVAIL;
}
