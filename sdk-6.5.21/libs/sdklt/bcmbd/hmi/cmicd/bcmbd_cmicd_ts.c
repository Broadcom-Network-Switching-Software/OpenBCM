/*! \file bcmbd_cmicd_ts.c
 *
 * HMI-specific TimeSync driver for CMICd.
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

#include <bcmbd/bcmbd_ts_internal.h>
#include <bcmbd/bcmbd_cmicd_acc.h>
#include <bcmbd/bcmbd_cmicd_ts_drv_internal.h>

/*******************************************************************************
 * Local definitions
 */

#define NUM_TS_INST 2

/*! Private data used in this driver */
typedef struct cmicd_ts_drv_data_s {

    /*! TS Access lock */
    sal_mutex_t ts_lock[NUM_TS_INST];

} cmicd_ts_drv_data_t;

static cmicd_ts_drv_data_t *ts_drv_data[BCMDRD_CONFIG_MAX_UNITS];

/*! TS access lock */
#define TS_LOCK(_p, _i) sal_mutex_take((_p)->ts_lock[_i], SAL_MUTEX_FOREVER)
#define TS_UNLOCK(_p, _i) sal_mutex_give((_p)->ts_lock[_i])

/*******************************************************************************
 * Private functions
 */

static int
timestamp_enable_get(int unit, int ts_inst, bool *enabled)
{
    CMIC_TIMESYNC_TS_COUNTER_ENABLEr_t ctr_en;

    if (READ_CMIC_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, &ctr_en) < 0) {
        return -1;
    }
    *enabled = (CMIC_TIMESYNC_TS_COUNTER_ENABLEr_ENABLEf_GET(ctr_en) != 0);

    return 0;
}

static int
timestamp_enable_set(int unit, int ts_inst, bool enable)
{
    int ioerr = 0;
    CMIC_TIMESYNC_TS_FREQ_CTRL_FRACr_t frac;
    CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr_t upr;
    CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr_t lwr;
    CMIC_TIMESYNC_TS_COUNTER_ENABLEr_t ctr_en;

    if (enable) {
        /* 250 Mhz - 4.000 ns */
        ioerr += READ_CMIC_TIMESYNC_TS_FREQ_CTRL_FRACr(unit, ts_inst, &frac);
        CMIC_TIMESYNC_TS_FREQ_CTRL_FRACr_FRACf_SET(frac, 0x40000000);
        ioerr += WRITE_CMIC_TIMESYNC_TS_FREQ_CTRL_FRACr(unit, ts_inst, frac);

        ioerr += READ_CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr(unit, ts_inst, &upr);
        CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr_NSf_SET(upr, 0);
        ioerr += WRITE_CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr(unit, ts_inst, upr);

        ioerr += READ_CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr(unit, ts_inst, &lwr);
        CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr_NSf_SET(lwr, 0);
        ioerr += WRITE_CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr(unit, ts_inst, lwr);
    }

    /* Enable/Disable timestamp generation */
    ioerr += READ_CMIC_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, &ctr_en);
    CMIC_TIMESYNC_TS_COUNTER_ENABLEr_ENABLEf_SET(ctr_en, enable);
    ioerr += WRITE_CMIC_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, ctr_en);

    return ioerr;
}

static int
timestamp_get(int unit, int ts_inst, uint64_t *timestamp)
{
    int ioerr = 0;
    uint64_t timestamp_upr, timestamp_lwr;
    CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr_t upr;
    CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr_t lwr;

    /* Capture timestamp immediately from registers instead of FIFO */
    ioerr += READ_CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr(unit, ts_inst, &lwr);
    ioerr += READ_CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr(unit, ts_inst, &upr);

    timestamp_lwr = CMIC_TIMESYNC_TS_FREQ_CTRL_LOWERr_NSf_GET(lwr);
    timestamp_upr = CMIC_TIMESYNC_TS_FREQ_CTRL_UPPERr_NSf_GET(upr);
    *timestamp = (timestamp_upr << 32) | timestamp_lwr;

    return ioerr;
}

static int
cmicd_ts_timestamp_enable_get(int unit, int ts_inst, bool *enable)
{
    cmicd_ts_drv_data_t *drv_data = ts_drv_data[unit];
    int ioerr = 0;

    if (drv_data == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (enable) {
        TS_LOCK(drv_data, ts_inst);

        ioerr += timestamp_enable_get(unit, ts_inst, enable);

        TS_UNLOCK(drv_data, ts_inst);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicd_ts_timestamp_enable_set(int unit, int ts_inst, bool enable)
{
    cmicd_ts_drv_data_t *drv_data = ts_drv_data[unit];
    int ioerr = 0;

    if (drv_data == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    TS_LOCK(drv_data, ts_inst);

    ioerr += timestamp_enable_set(unit, ts_inst, enable);

    TS_UNLOCK(drv_data, ts_inst);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
cmicd_ts_timestamp_get(int unit, int ts_inst, uint64_t *timestamp,
                       uint32_t *valid_bits, uint32_t *frac_bits)
{
    cmicd_ts_drv_data_t *drv_data = ts_drv_data[unit];
    int ioerr = 0;
    bool enable = false;

    if (drv_data == NULL) {
        return SHR_E_INIT;
    }

    if (ts_inst < 0 || ts_inst >= NUM_TS_INST) {
        return SHR_E_PARAM;
    }

    if (valid_bits) {
        *valid_bits = 48;
    }
    if (frac_bits) {
        *frac_bits = 0;
    }
    if (timestamp) {
        TS_LOCK(drv_data, ts_inst);

        ioerr += timestamp_enable_get(unit, ts_inst, &enable);
        ioerr += timestamp_get(unit, ts_inst, timestamp);

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
cmicd_ts_cleanup(int unit)
{
    cmicd_ts_drv_data_t *drv_data = ts_drv_data[unit];
    int idx;

    if (drv_data == NULL) {
        return SHR_E_NONE;
    }

    ts_drv_data[unit] = NULL;

    for (idx = 0; idx < NUM_TS_INST; idx++) {
        if (drv_data->ts_lock[idx]) {
            sal_mutex_destroy(drv_data->ts_lock[idx]);
        }
    }

    sal_free(drv_data);

    return SHR_E_NONE;
}

static int
cmicd_ts_init(int unit)
{
    cmicd_ts_drv_data_t *drv_data;
    int idx;

    drv_data = sal_alloc(sizeof(*drv_data), "bcmbdCmicdTsDrvData");
    if (drv_data == NULL) {
        return SHR_E_FAIL;
    }
    sal_memset(drv_data, 0, sizeof(*drv_data));

    ts_drv_data[unit] = drv_data;

    for (idx = 0; idx < NUM_TS_INST; idx++) {
        drv_data->ts_lock[idx] = sal_mutex_create("bdCmicdTsLock");
        if (drv_data->ts_lock[idx] == NULL) {
            cmicd_ts_cleanup(unit);
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

/* HMI-specific TimeSync driver. */
static bcmbd_ts_drv_t cmicd_ts_drv = {

    /*! Initialize TimeSync driver. */
    .init = cmicd_ts_init,

    /*! Clean up TimeSync driver. */
    .cleanup = cmicd_ts_cleanup,

    /*! Get enable status of the timestamp counter. */
    .timestamp_enable_get = cmicd_ts_timestamp_enable_get,

    /*! Enable/Disable the timestamp counter. */
    .timestamp_enable_set = cmicd_ts_timestamp_enable_set,

    /*! Get instantaneous value of timestamp counter. */
    .timestamp_get = cmicd_ts_timestamp_get
};

/*******************************************************************************
 * Public function
 */

int
bcmbd_cmicd_ts_drv_init(int unit)
{
    return bcmbd_ts_drv_init(unit, &cmicd_ts_drv);
}

int
bcmbd_cmicd_ts_drv_cleanup(int unit)
{
    return bcmbd_ts_drv_init(unit, NULL);
}
