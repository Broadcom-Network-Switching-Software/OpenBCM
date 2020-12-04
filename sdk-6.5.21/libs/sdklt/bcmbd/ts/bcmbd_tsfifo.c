/*! \file bcmbd_ts_fifo.c
 *
 * BD TimeSync FIFO driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_chip.h>

#include <bcmbd/bcmbd_tsfifo_internal.h>

/*******************************************************************************
 * Local definitions
 */

static const bcmbd_tsfifo_drv_t *bd_tsfifo_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmbd_tsfifo_drv_init(int unit, const bcmbd_tsfifo_drv_t *tsfifo_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_tsfifo_drv[unit] = tsfifo_drv;

    return SHR_E_NONE;
}

int
bcmbd_tsfifo_capture_start(int unit, bcmbd_tsfifo_capture_cb_t *cb)
{
    const bcmbd_tsfifo_drv_t *td;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    td = bd_tsfifo_drv[unit];
    if (td && td->capture_start) {
        return td->capture_start(unit, cb);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_tsfifo_capture_stop(int unit)
{
    const bcmbd_tsfifo_drv_t *td;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    td = bd_tsfifo_drv[unit];
    if (td && td->capture_stop) {
        return td->capture_stop(unit);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_tsfifo_capture_dump(
        int unit,
        bcmbd_tsfifo_dump_action_f *cb,
        bcmbd_tsfifo_dump_cb_t *app_cb)
{
    const bcmbd_tsfifo_drv_t *td;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    td = bd_tsfifo_drv[unit];
    if (td && td->capture_dump) {
        return td->capture_dump(cb, app_cb);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_tsfifo_cleanup(int unit)
{
    const bcmbd_tsfifo_drv_t *td;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    td = bd_tsfifo_drv[unit];
    if (td && td->cleanup) {
        return td->cleanup(unit);
    }
    return SHR_E_UNAVAIL;
}

int
bcmbd_tsfifo_init(int unit)
{
    const bcmbd_tsfifo_drv_t *td;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    td = bd_tsfifo_drv[unit];
    if (td && td->init) {
        return td->init(unit);
    }
    return SHR_E_UNAVAIL;
}
