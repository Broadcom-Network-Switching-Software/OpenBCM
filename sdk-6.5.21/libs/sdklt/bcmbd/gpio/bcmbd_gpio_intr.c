/*! \file bcmbd_gpio_intr.c
 *
 * GPIO interrupt driver API.
 *
 * This API can be used when multiple GPIO interrupts are tied to
 * a single hardware interrupt.
 *
 * Each GPIO interrupt client will register a handler for a given
 * GPIO interrupt source.
 *
 * For each GPIO interrupt source, the hardware interrupt handler
 * will clear the interrupt condition and call the registered GPIO
 * interrupt handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_gpio_intr_internal.h>


/*******************************************************************************
* Local definitions
 */

static const bcmbd_intr_drv_t *bd_gpio_intr_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
* Public functions
 */

int
bcmbd_gpio_intr_drv_init(int unit, const bcmbd_intr_drv_t *sw_intr_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_gpio_intr_drv[unit] = sw_intr_drv;

    return SHR_E_NONE;
}

int
bcmbd_gpio_intr_func_set(int unit, unsigned int intr_num,
                         bcmbd_intr_f intr_func, uint32_t intr_param)
{
    const bcmbd_intr_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_gpio_intr_drv[unit];
    if (drv && drv->intr_func_set) {
        return drv->intr_func_set(unit, intr_num, intr_func, intr_param);
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_intr_enable(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_gpio_intr_drv[unit];
    if (drv && drv->intr_enable) {
        drv->intr_enable(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_intr_disable(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_gpio_intr_drv[unit];
    if (drv && drv->intr_disable) {
        drv->intr_disable(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_intr_clear(int unit, unsigned int intr_num)
{
    const bcmbd_intr_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_gpio_intr_drv[unit];
    if (drv && drv->intr_clear) {
        drv->intr_clear(unit, intr_num);
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}
