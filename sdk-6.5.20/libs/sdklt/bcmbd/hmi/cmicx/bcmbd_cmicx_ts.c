/*! \file bcmbd_cmicx_ts.c
 *
 * HMI-specific TimeSync driver for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_ts_internal.h>
#include <bcmbd/bcmbd_cmicx_ts_drv_internal.h>

/*******************************************************************************
 * Local definitions
 */

#define NUM_TS_INST 2

/*! Private data used in this driver */
typedef struct cmicx_ts_drv_data_s {

    /*! TS Access lock */
    sal_mutex_t ts_lock[NUM_TS_INST];

} cmicx_ts_drv_data_t;

static cmicx_ts_drv_data_t *ts_drv_data[BCMDRD_CONFIG_MAX_UNITS];

static const bcmbd_cmicx_ts_drv_acc_t *ts_acc[BCMDRD_CONFIG_MAX_UNITS];

/*! TS access lock */
#define TS_LOCK(_p, _i) sal_mutex_take((_p)->ts_lock[_i], SAL_MUTEX_FOREVER)
#define TS_UNLOCK(_p, _i) sal_mutex_give((_p)->ts_lock[_i])

/*******************************************************************************
 * Private functions
 */

static int
cmicx_ts_timestamp_enable_get(int unit, int ts_inst, bool *enable)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (enable) {
        TS_LOCK(drv_data, ts_inst);

        ioerr += acc->timestamp_enable_get(unit, ts_inst, enable);

        TS_UNLOCK(drv_data, ts_inst);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicx_ts_timestamp_enable_set(int unit, int ts_inst, bool enable)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    TS_LOCK(drv_data, ts_inst);

    ioerr += acc->timestamp_enable_set(unit, ts_inst, enable);

    TS_UNLOCK(drv_data, ts_inst);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicx_ts_timestamp_get(int unit, int ts_inst, uint64_t *timestamp,
                       uint32_t *valid_bits, uint32_t *frac_bits)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;
    bool enable = false;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (valid_bits) {
        *valid_bits = 52;
    }
    if (frac_bits) {
        *frac_bits = 4;
    }
    if (timestamp) {
        TS_LOCK(drv_data, ts_inst);

        ioerr += acc->timestamp_enable_get(unit, ts_inst, &enable);
        ioerr += acc->timestamp_get(unit, ts_inst, timestamp);

        TS_UNLOCK(drv_data, ts_inst);

        if (ioerr) {
            return SHR_E_IO;
        }
        if (!enable) {
            return SHR_E_DISABLED;
        }
    }

    return SHR_E_NONE;
}

static int
cmicx_ts_timestamp_set(int unit, int ts_inst, uint64_t timestamp,
                       uint32_t valid_bits, uint32_t frac_bits)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (valid_bits != 52 || frac_bits != 4) {
        return SHR_E_PARAM;
    }

    TS_LOCK(drv_data, ts_inst);

    ioerr += acc->timestamp_set(unit, ts_inst, timestamp);

    TS_UNLOCK(drv_data, ts_inst);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicx_ts_timestamp_offset_get(int unit, int ts_inst, uint64_t *ts_offset)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (ts_offset) {
        TS_LOCK(drv_data, ts_inst);

        ioerr += acc->timestamp_offset_get(unit, ts_inst, ts_offset);

        TS_UNLOCK(drv_data, ts_inst);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicx_ts_timestamp_offset_set(int unit, int ts_inst,
                              uint64_t ts_offset, uint64_t update_interval)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int ioerr = 0;
    bool enable = false;
    uint64_t timestamp, updns;

    if (drv_data == NULL || acc == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    TS_LOCK(drv_data, ts_inst);

    ioerr += acc->timestamp_enable_get(unit, ts_inst, &enable);
    if (enable) {
        ioerr += acc->timestamp_get(unit, ts_inst, &timestamp);
        if (ioerr == 0) {
            updns = (timestamp + update_interval) >> 4;
            ioerr += acc->timestamp_offset_set(unit, ts_inst, ts_offset, updns);
        }
    }

    TS_UNLOCK(drv_data, ts_inst);

    if (ioerr) {
        return SHR_E_IO;
    }
    if (!enable) {
        return SHR_E_DISABLED;
    }
    return SHR_E_NONE;
}

static int
cmicx_ts_cleanup(int unit)
{
    cmicx_ts_drv_data_t *drv_data = ts_drv_data[unit];
    int idx;

    ts_drv_data[unit] = NULL;
    ts_acc[unit] = NULL;

    if (drv_data == NULL) {
        return SHR_E_NONE;
    }

    for (idx = 0; idx < NUM_TS_INST; idx++) {
        if (drv_data->ts_lock[idx]) {
            sal_mutex_destroy(drv_data->ts_lock[idx]);
        }
    }

    sal_free(drv_data);

    return SHR_E_NONE;
}

static int
cmicx_ts_init(int unit)
{
    cmicx_ts_drv_data_t *drv_data;
    const bcmbd_cmicx_ts_drv_acc_t *acc = ts_acc[unit];
    int idx;

    if (acc == NULL) {
        return SHR_E_IO;
    }

    drv_data = sal_alloc(sizeof(*drv_data), "bcmbdCmicxTsDrvData");
    if (drv_data == NULL) {
        return SHR_E_FAIL;
    }
    sal_memset(drv_data, 0, sizeof(*drv_data));

    ts_drv_data[unit] = drv_data;

    for (idx = 0; idx < NUM_TS_INST; idx++) {
        drv_data->ts_lock[idx] = sal_mutex_create("bdCmicxTsLock");
        if (drv_data->ts_lock[idx] == NULL) {
            cmicx_ts_cleanup(unit);
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

/* HMI-specific TimeSync driver. */
static bcmbd_ts_drv_t cmicx_ts_drv = {

    /*! Initialize TimeSync driver. */
    .init = cmicx_ts_init,

    /*! Clean up TimeSync driver. */
    .cleanup = cmicx_ts_cleanup,

    /*! Get enable status of the timestamp counter. */
    .timestamp_enable_get = cmicx_ts_timestamp_enable_get,

    /*! Enable/Disable the timestamp counter. */
    .timestamp_enable_set = cmicx_ts_timestamp_enable_set,

    /*! Get instantaneous value of timestamp counter. */
    .timestamp_get = cmicx_ts_timestamp_get,

    /*! Set instantaneous value of timestamp counter. */
    .timestamp_set = cmicx_ts_timestamp_set,

    /*! Get offset of timestamp counter. */
    .timestamp_offset_get = cmicx_ts_timestamp_offset_get,

    /*! Set offset of timestamp counter. */
    .timestamp_offset_set = cmicx_ts_timestamp_offset_set
};

/*******************************************************************************
 * Public function
 */

int
bcmbd_cmicx_ts_drv_init(int unit, const bcmbd_cmicx_ts_drv_acc_t *acc)
{
    int rv;

    rv = bcmbd_ts_drv_init(unit, &cmicx_ts_drv);
    if (SHR_SUCCESS(rv)) {
        ts_acc[unit] = acc;
    }

    return rv;
}

int
bcmbd_cmicx_ts_drv_cleanup(int unit)
{
    int rv;

    rv = bcmbd_ts_drv_init(unit, NULL);
    if (SHR_SUCCESS(rv)) {
        ts_acc[unit] = NULL;
    }

    return rv;
}
