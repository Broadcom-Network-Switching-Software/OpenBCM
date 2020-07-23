/*! \file bcmbd_dev.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

static bcmbd_dev_reset_cb_f bcmbd_reset_cb[BCMDRD_CONFIG_MAX_UNITS];
static bool bcmbd_running[BCMDRD_CONFIG_MAX_UNITS];

int
bcmbd_dev_reset(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);
    bcmbd_dev_reset_cb_f reset_cb;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Device reset\n")));

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    /* Optional reset callback */
    reset_cb = bcmbd_reset_cb[unit];
    if (reset_cb) {
        int rv = reset_cb(unit);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Device reset callback: %s\n"),
                  shr_errmsg(rv)));
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    if (bd->dev_reset == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->dev_reset(unit);
}

int
bcmbd_dev_reset_cb_set(int unit, bcmbd_dev_reset_cb_f reset_cb)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bcmbd_reset_cb[unit] = reset_cb;

    return SHR_E_NONE;
}

int
bcmbd_dev_init(int unit)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Device init\n")));

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->dev_init == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->dev_init(unit);
}

int
bcmbd_dev_start(int unit)
{
    int rv;
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Device start\n")));

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    /* Optional function */
    rv = SHR_E_NONE;
    if (bcmbd_running[unit]) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Base driver already running\n")));
    } else if (bd->dev_start) {
        rv = bd->dev_start(unit);
    }

    /* Save the state */
    if (SHR_SUCCESS(rv)) {
        bcmbd_running[unit] = true;
    }

    return rv;
}

int
bcmbd_dev_stop(int unit)
{
    int rv;
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Device stop\n")));

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    /* Optional function */
    rv = SHR_E_NONE;
    if (!bcmbd_running[unit]) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Base driver not running\n")));
    } else if (bd->dev_stop) {
        rv = bd->dev_stop(unit);
    }

    /* Save the state */
    if (SHR_SUCCESS(rv)) {
        bcmbd_running[unit] = false;
    }

    return rv;
}

bool
bcmbd_dev_running(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }
    return bcmbd_running[unit];
}
