/*! \file bcmbd_led.c
 *
 * Top-level LED API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_led_internal.h>

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local variables
 */

static const bcmbd_led_drv_t *bd_led_drv[BCMDRD_CONFIG_MAX_UNITS] = {NULL};
static bcmbd_led_dev_t bd_led_dev[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_led_drv_init(int unit, const bcmbd_led_drv_t *led_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (!bcmdrd_feature_is_real_hw(unit)) {
        return SHR_E_NONE;
    }

    bd_led_drv[unit] = led_drv;

    return SHR_E_NONE;
}

int
bcmbd_led_dev_get(int unit, bcmbd_led_dev_t **led_dev)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    *led_dev = &bd_led_dev[unit];

    return SHR_E_NONE;
}

int
bcmbd_led_init(int unit)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->init) {
        return led_drv->init(unit);
    }

    return SHR_E_NONE;
}

int
bcmbd_led_cleanup(int unit)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->cleanup) {
        return led_drv->cleanup(unit);
    }
    return SHR_E_NONE;
}


int
bcmbd_led_uc_fw_load(int unit, int led_uc, const uint8_t *buf, int len)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->fw_load) {
        return led_drv->fw_load(unit, led_uc, buf, len);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_led_uc_num_get(int unit)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_num_get) {
        return led_drv->uc_num_get(unit);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_led_uc_start(int unit, int led_uc)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_start_set) {
        return led_drv->uc_start_set(unit, led_uc, 1);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_led_uc_stop(int unit, int led_uc)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_start_set) {
        return led_drv->uc_start_set(unit, led_uc, 0);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_led_uc_start_get(int unit, int led_uc,
                       int *start)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_start_get) {
        return led_drv->uc_start_get(unit, led_uc, start);
    }
    return SHR_E_UNAVAIL;


}

int
bcmbd_led_uc_swdata_write(int unit, int led_uc, int offset, uint8_t value)
{

    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_swdata_write) {
        return led_drv->uc_swdata_write(unit, led_uc, offset, value);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_led_uc_swdata_read(int unit, int led_uc, int offset, uint8_t *value)
{

    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->uc_swdata_read) {
        return led_drv->uc_swdata_read(unit, led_uc, offset, value);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_port_to_led_uc_port(int unit, int port, int *led_uc, int *led_uc_port)
{
    const bcmbd_led_drv_t *led_drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    led_drv = bd_led_drv[unit];
    if (led_drv && led_drv->pport_to_led_uc_port) {
        return led_drv->pport_to_led_uc_port(unit, port, led_uc, led_uc_port);
    }
    return SHR_E_UNAVAIL;
}
