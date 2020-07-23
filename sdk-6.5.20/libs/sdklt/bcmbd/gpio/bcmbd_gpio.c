/*! \file bcmbd_gpio.c
 *
 * BD GPIO driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_chip.h>

#include <bcmbd/bcmbd_gpio_internal.h>


/*******************************************************************************
 * Local definitions
 */

static const bcmbd_gpio_drv_t *bd_gpio_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Public functions
 */

int
bcmbd_gpio_drv_init(int unit, const bcmbd_gpio_drv_t *gpio_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_gpio_drv[unit] = gpio_drv;

    return SHR_E_NONE;
}

int
bcmbd_gpio_init(int unit)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->init) {
        return gpio_drv->init(unit);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_cleanup(int unit)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->cleanup) {
        return gpio_drv->cleanup(unit);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_mode_set(int unit, uint32_t gpio, bcmbd_gpio_mode_t mode)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->mode_set) {
        return gpio_drv->mode_set(unit, gpio, mode);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_mode_get(int unit, uint32_t gpio, bcmbd_gpio_mode_t *mode)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->mode_get) {
        return gpio_drv->mode_get(unit, gpio, mode);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_set(int unit, uint32_t gpio, bool val)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->set) {
        return gpio_drv->set(unit, gpio, val);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_gpio_get(int unit, uint32_t gpio, bool *val)
{
    const bcmbd_gpio_drv_t *gpio_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    gpio_drv = bd_gpio_drv[unit];
    if (gpio_drv && gpio_drv->get) {
        return gpio_drv->get(unit, gpio, val);
    }
    return SHR_E_UNAVAIL;
}
