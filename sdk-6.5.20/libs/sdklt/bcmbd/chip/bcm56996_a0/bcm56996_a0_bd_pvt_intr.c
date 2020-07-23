/*! \file bcm56996_a0_bd_pvt_intr.c
 *
 * PVTMON thermal monitor interrupt driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_time.h>
#include <sal/sal_sleep.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_spinlock.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_pvt_intr_internal.h>
#include <bcmbd/bcmbd_pvt_internal.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmbd/chip/bcm56996_a0_lp_intr.h>
#include <bcmbd/chip/bcm56996_a0_pvt_intr.h>

#include "bcm56996_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */
#define MAX_UNITS                   BCMDRD_CONFIG_MAX_UNITS
#define MAX_INTRS                   MAX_PVT_INTR
#define INTR_NUM_TO_MON_IDX(_num)   ((_num)>>1)

#define PVT_SENSOR_CNT              16

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(pvt_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(pvt_intr_lock[_u]);

typedef struct bcmbd_intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} bcmbd_intr_vect_t;

typedef struct bcmbd_intr_vect_info_s {
    bcmbd_intr_vect_t intr_vects[MAX_INTRS];
} bcmbd_intr_vect_info_t;

/*******************************************************************************
 * Local data
 */

static bool pvt_intr_connected[MAX_UNITS];
static shr_thread_t pvt_thread_ctrl[MAX_UNITS];
static sal_spinlock_t pvt_intr_lock[MAX_UNITS];
static bcmbd_intr_vect_info_t pvt_intr_vect_info[MAX_UNITS];

/*******************************************************************************
 * Private functions
 */


static double
pvt_temp_convert(bcmbd_pvt_equation_t equation, uint32_t data)
{
    /*
     * Equation type 1: Linear
     * Temperature = -0.23751*data + 356.01
     *
     * Equation type 2: Quadratic
     * Temperature = -8.4666e-06*(data)^2 - 0.21519*(data) + 341.48
     */
    double var = 0;

    if (equation == PVT_EQUATION_1) {
        var = -0.23751*data + 356.01;
    } else if (equation == PVT_EQUATION_2) {
        var = data * data;
        var = 0.0000084666 * var;
        var = 0 - var;
        var -= 0.21519*data;
        var += 341.48;
    } else if (equation == PVT_EQUATION_3) {
        /* place holder for new equation */
    }

    return var;
}


static void
pvt_config_dump(int unit)
{
    int ioerr = 0;
    TOP_PVTMON_INTR_STATUS_0r_t reg;
    TOP_PVTMON_RESULT_1r_t result_1;
    TOP_PVTMON_RESULT_0r_t hist;
    TOP_PVTMON_INTR_THRESHOLDr_t threshold;
    uint32_t mask;
    int ix;
    int h_min_data[16], h_max_data[16];
    bcmbd_temperature_t h_min_temp[16], h_max_temp[16];
    int c_data[16], t_min_data[16], t_max_data[16];
    bcmbd_temperature_t c_temp[16], t_min_temp[16], t_max_temp[16];
    TOP_PVTMON_INTR_MASK_0r_t intr_mask;
    TOP_TMON_CHANNELS_CTRL_1r_t hw_reset;
    TOP_PVTMON_HW_RST_THRESHOLDr_t hw_threshold;
    int intr_data;
    int hw_reset_en;
    bcmbd_temperature_t hw_threshold_t[16];
    int hw_threshold_d[16];

    TOP_PVTMON_INTR_MASK_0r_CLR(intr_mask);
    ioerr += READ_TOP_PVTMON_INTR_MASK_0r(unit, &intr_mask);
    intr_data = TOP_PVTMON_INTR_MASK_0r_GET(intr_mask);

    TOP_TMON_CHANNELS_CTRL_1r_CLR(hw_reset);
    ioerr += READ_TOP_TMON_CHANNELS_CTRL_1r(unit, &hw_reset);
    hw_reset_en =
        TOP_TMON_CHANNELS_CTRL_1r_TMON_HW_RST_HIGHTEMP_CTRL_ENf_GET(hw_reset);

    ioerr += READ_TOP_PVTMON_INTR_STATUS_0r(unit, &reg);
    mask = TOP_PVTMON_INTR_STATUS_0r_GET(reg);
    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        /* Read out current temperature. */
        TOP_PVTMON_RESULT_1r_CLR(result_1);
        ioerr += READ_TOP_PVTMON_RESULT_1r(unit, ix, &result_1);
        c_data[ix] = TOP_PVTMON_RESULT_1r_PVT_DATAf_GET(result_1);
        c_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, c_data[ix]);

        /* Read out history high and low */
        TOP_PVTMON_RESULT_0r_CLR(hist);
        ioerr += READ_TOP_PVTMON_RESULT_0r(unit, ix, &hist);
        h_max_data[ix] = TOP_PVTMON_RESULT_0r_MAX_PVT_DATAf_GET(hist);
        h_min_data[ix] = TOP_PVTMON_RESULT_0r_MIN_PVT_DATAf_GET(hist);

        h_max_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, h_max_data[ix]);
        h_min_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, h_min_data[ix]);

        /* Read out interrupt threshold config. */
        TOP_PVTMON_INTR_THRESHOLDr_CLR(threshold);
        ioerr += READ_TOP_PVTMON_INTR_THRESHOLDr(unit, ix, &threshold);
        t_min_data[ix] =
            TOP_PVTMON_INTR_THRESHOLDr_MIN_THRESHOLDf_GET(threshold);
        t_max_data[ix] =
            TOP_PVTMON_INTR_THRESHOLDr_MAX_THRESHOLDf_GET(threshold);

        t_min_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, t_min_data[ix]);
        t_max_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, t_max_data[ix]);

        /* Read out hardware reset threshold config. */
        ioerr += READ_TOP_PVTMON_HW_RST_THRESHOLDr(unit, ix, &hw_threshold);
        hw_threshold_d[ix] =
            TOP_PVTMON_HW_RST_THRESHOLDr_MIN_HW_RST_THRESHOLDf_GET(hw_threshold);
        hw_threshold_t[ix] =
            pvt_temp_convert(PVT_EQUATION_1, hw_threshold_d[ix]);
    }

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
        return;
    }

    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Intr status: 0x%x, Intr mask: 0x%x, "
                         "HW reset en: 0x%x\n\n"),
              mask, intr_data, hw_reset_en));

    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Sensor %d, Temp: %.1f C (0x%x), "
                             "hmax: %.1f C (0x%x), "
                             "hmin: %.1f C (0x%x)\n"),
                  ix, c_temp[ix], c_data[ix], h_min_temp[ix],
                  h_min_data[ix], h_max_temp[ix], h_max_data[ix]));

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Sensor %d, INTR threshold, Min: %.1f C (0x%x), "
                             "Max: %.1f C (0x%x)\n"),
                  ix, t_min_temp[ix], t_min_data[ix],
                  t_max_temp[ix], t_max_data[ix]));

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Sensor %d, HW reset threshold: %.1f C (0x%x)\n"),
                  ix, hw_threshold_t[ix], hw_threshold_d[ix]));
    }
}


static void
pvt_intr_update(int unit, unsigned int intr_num, int enable)
{
    TOP_PVTMON_INTR_MASK_0r_t intr_mask;
    TOP_TMON_CHANNELS_CTRL_1r_t chan_ctrl;
    uint32_t mask;
    uint32_t sw;
    int mon_idx;
    int ioerr = 0;

    mon_idx = INTR_NUM_TO_MON_IDX(intr_num);

    ioerr += READ_TOP_TMON_CHANNELS_CTRL_1r(unit, &chan_ctrl);
    sw = TOP_TMON_CHANNELS_CTRL_1r_TMON_INTR_HIGHTEMP_CTRL_ENf_GET(chan_ctrl);
    if (enable) {
        sw |= LSHIFT32(1, mon_idx);
    } else {
        sw &= ~LSHIFT32(1, mon_idx);
    }
    TOP_TMON_CHANNELS_CTRL_1r_TMON_INTR_HIGHTEMP_CTRL_ENf_SET(chan_ctrl, sw);
    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_1r(unit, chan_ctrl);

    ioerr += READ_TOP_PVTMON_INTR_MASK_0r(unit, &intr_mask);
    mask = TOP_PVTMON_INTR_MASK_0r_GET(intr_mask);
    if (enable) {
        mask |= LSHIFT32(1, intr_num);
    } else {
        mask &= ~LSHIFT32(1, intr_num);
    }
    TOP_PVTMON_INTR_MASK_0r_SET(intr_mask, mask);
    ioerr += WRITE_TOP_PVTMON_INTR_MASK_0r(unit, intr_mask);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}

static void
pvt_intr_enable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Enable PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    pvt_intr_update(unit, intr_num, 1);
    INTR_MASK_UNLOCK(unit);
}

static void
pvt_intr_disable(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Disable PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    /* We may be called by multiple application threads */
    INTR_MASK_LOCK(unit);
    pvt_intr_update(unit, intr_num, 0);
    INTR_MASK_UNLOCK(unit);
}

static void
pvt_intr(int unit)
{
    uint32_t intr_stat = 0;
    uint32_t intr_num;
    TOP_PVTMON_INTR_STATUS_0r_t reg;

    if (READ_TOP_PVTMON_INTR_STATUS_0r(unit, &reg) != 0) {
        return;
    }
    intr_stat = TOP_PVTMON_INTR_STATUS_0r_GET(reg);
    if (intr_stat == 0) {
        /* No active interrupts */
        return;
    }

    
    pvt_config_dump(unit);

    /* Now call interrupt handlers according to the interrupt source matrix */
    for (intr_num = 0; intr_num < MAX_INTRS; intr_num++) {
        if ((intr_stat & LSHIFT32(1, intr_num))) {
            bcmbd_intr_vect_t *iv;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVT interrupt %u\n"),
                       intr_num));
            iv = &pvt_intr_vect_info[unit].intr_vects[intr_num];
            if (iv->func) {
                uint32_t param = iv->param;
                iv->func(unit, param);
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "No handler for PVT interrupt %u\n"),
                           intr_num));
                pvt_intr_disable(unit, intr_num);
            }
        }
    }
}



/*******************************************************************************
 * PVT Interrupt driver
 */

static void
pvt_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }

        pvt_intr(unit);
        bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);
    }
}

static void
pvt_isr(int unit, uint32_t param)
{
    /* Disable interrupt and wake up thread */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PVTMON_INTR);

    shr_thread_wake(pvt_thread_ctrl[unit]);
}

static void
pvt_intr_clear(int unit, unsigned int intr_num)
{
    int ioerr = 0;
    uint32_t intr_bits;
    TOP_PVTMON_INTR_STATUS_0r_t status;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                          "Clear PVTMON intr %u\n"),
                 intr_num));
    if (intr_num >= MAX_INTRS) {
        return;
    }

    intr_bits = LSHIFT32(1, intr_num);

    /* Write 1 to clear status bits to dessert interrupt */
    TOP_PVTMON_INTR_STATUS_0r_CLR(status);
    TOP_PVTMON_INTR_STATUS_0r_SET(status, intr_bits);
    ioerr += WRITE_TOP_PVTMON_INTR_STATUS_0r(unit, status);
    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}


static int
pvt_intr_init(int unit)
{
    shr_thread_t tc;
    void *arg;

    if (pvt_intr_lock[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    pvt_intr_lock[unit] = sal_spinlock_create("bcmbdPvtIntr");
    if (pvt_intr_lock[unit] == NULL) {
        return SHR_E_FAIL;
    }

    if (pvt_thread_ctrl[unit] != NULL) {
        return SHR_E_INTERNAL;
    }

    /* Pass in unit number as context */
    arg = (void *)(uintptr_t)unit;

    tc = shr_thread_start("bcmbdPvtIntr", -1, pvt_thread, arg);
    if (tc == NULL) {
        return SHR_E_FAIL;
    }

    pvt_thread_ctrl[unit] = tc;

    return SHR_E_NONE;
}

static int
pvt_intr_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    bcmbd_cmicx_lp_intr_disable(unit, PVTMON_INTR);
    pvt_intr_connected[unit] = false;

    if (pvt_thread_ctrl[unit] != NULL) {
        rv = shr_thread_stop(pvt_thread_ctrl[unit], 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        pvt_thread_ctrl[unit] = NULL;
    }

    if (pvt_intr_lock[unit] != NULL) {
        rv = sal_spinlock_destroy(pvt_intr_lock[unit]);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        pvt_intr_lock[unit] = NULL;
    }

    return rv;
}

static int
pvt_intr_func_set(int unit, unsigned int intr_num,
                  bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_vect_t *iv;

    if (!pvt_intr_connected[unit]) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add handler for PVTMON intr %u\n"),
                   intr_num));
        bcmbd_cmicx_lp_intr_func_set(unit, PVTMON_INTR, pvt_isr, 0);
        bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);
        pvt_intr_connected[unit] = true;
    }

    if (intr_num >= MAX_INTRS) {
        return SHR_E_PARAM;
    }

    iv = &pvt_intr_vect_info[unit].intr_vects[intr_num];
    iv->func = intr_func;
    iv->param = intr_param;

    return SHR_E_NONE;
}


static bcmbd_intr_drv_t pvt_intr_drv = {
    .intr_func_set = pvt_intr_func_set,
    .intr_enable = pvt_intr_enable,
    .intr_disable = pvt_intr_disable,
    .intr_clear = pvt_intr_clear,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56996_a0_bd_pvt_intr_drv_init(int unit)
{
    int rv = SHR_E_NONE;
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    rv = bcmbd_pvt_intr_drv_init(unit, &pvt_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = pvt_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PVT intr init error\n")));
            (void)pvt_intr_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56996_a0_bd_pvt_intr_drv_cleanup(int unit)
{
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (emul) {
        return SHR_E_NONE;
    } else {
        (void)bcmbd_pvt_intr_drv_init(unit, NULL);
        return pvt_intr_cleanup(unit);
    }
}
