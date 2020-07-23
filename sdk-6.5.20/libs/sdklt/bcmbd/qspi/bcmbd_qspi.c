/*! \file bcmbd_qspi.c
 *
 * BD QSPI driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_qspi.h>
#include <bcmbd/bcmbd_qspi_internal.h>


/*******************************************************************************
 * Local definitions
 */

/*! Transfer flag to process all data in one time. */
#define BD_QSPI_TRANS_ONE (BCMBD_QSPI_TRANS_BEGIN | BCMBD_QSPI_TRANS_END)

static bcmbd_qspi_drv_t *bd_qspi_drv[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Public functions
 */

int
bcmbd_qspi_drv_init(int unit, bcmbd_qspi_drv_t *drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bd_qspi_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmbd_qspi_init(int unit, bcmbd_qspi_conf_t *conf)
{
    bcmbd_qspi_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_qspi_drv[unit];
    if (drv && drv->init) {
        return drv->init(unit, conf);
    }

    return SHR_E_NONE;
}

int
bcmbd_qspi_cleanup(int unit)
{
    bcmbd_qspi_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_qspi_drv[unit];
    if (!drv) {
        return SHR_E_NONE;
    }

    if (drv && drv->cleanup) {
        drv->cleanup(unit);
    }

    return SHR_E_NONE;
}

int
bcmbd_qspi_transfer(int unit, uint32_t flag,
                    size_t wlen, uint8_t *wbuf,
                    size_t rlen, uint8_t *rbuf)
{
    const bcmbd_qspi_drv_t *drv;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    drv = bd_qspi_drv[unit];
    if (drv && drv->transfer) {
        return drv->transfer(unit, flag, wlen, wbuf, rlen, rbuf);
    }

    return SHR_E_NONE;
}

int
bcmbd_qspi_transfer_one(int unit,
                        size_t wlen, uint8_t *wbuf,
                        size_t rlen, uint8_t *rbuf)
{
    return bcmbd_qspi_transfer(unit, BD_QSPI_TRANS_ONE, wlen, wbuf, rlen, rbuf);
}
