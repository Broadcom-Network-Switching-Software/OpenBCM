/*! \file bcmbd_ts_intr.c
 *
 * Timesync interrupt driver API.
 *
 * This API can be used when multiple timesync interrupts are tied to
 * a single hardware interrupt.
 *
 * Each timesync interrupt client will register a handler for a given
 * timesync interrupt source.
 *
 * For each timesync interrupt source, the hardware interrupt handler
 * will clear the interrupt condition and call the registered timesync
 * interrupt handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_ts_intr_internal.h>

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
* Local definitions
 */

static const bcmbd_intr_drv_t *bd_ts_intr_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmbd_ts_intr_drv_init(int unit, const bcmbd_intr_drv_t *sw_intr_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_ts_intr_drv[unit] = sw_intr_drv;

    return SHR_E_NONE;
}

int
bcmbd_ts_intr_func_set(int unit, unsigned int intr_num,
                       bcmbd_intr_f intr_func, uint32_t intr_param)
{
    const bcmbd_intr_drv_t *sd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    sd = bd_ts_intr_drv[unit];
    if (sd && sd->intr_func_set) {
        return sd->intr_func_set(unit, intr_num, intr_func, intr_param);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_intr_enable(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *sd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    sd = bd_ts_intr_drv[unit];
    if (sd && sd->intr_enable) {
        sd->intr_enable(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_intr_disable(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *sd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    sd = bd_ts_intr_drv[unit];
    if (sd && sd->intr_disable) {
        sd->intr_disable(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_ts_intr_clear(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *sd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    sd = bd_ts_intr_drv[unit];
    if (sd && sd->intr_clear) {
        sd->intr_clear(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}
