/*! \file bcm56880_a0_bd_pvt_intr.c
 *
 * PVTMON thermal monitor interrupt configuration.
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
#include <bcmbd/chip/bcm56880_a0_lp_intr.h>
#include <bcmbd/bcmbd_pvt_intr_internal.h>
#include <bcmbd/bcmbd_pvt_internal.h>
#include <bcmbd/chip/bcm56880_a0_acc.h>
#include <bcmbd/chip/bcm56880_a0_pvt.h>
#include "bcm56880_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

/*
 * BCM56880_A0 AVS sensor doesn't have hist min and max control
 * Keep a software copy here
 */
static bcmbd_avs_t avs_temp[BCMDRD_CONFIG_MAX_UNITS];

static shr_thread_t pvt_thread_ctrl[MAX_UNITS];
static sal_spinlock_t pvt_intr_lock[MAX_UNITS];

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

    }

    return var;
}

static void
pvt_intr_mask(int unit, int chan, int enable)
{
    int ioerr = 0;
    TOP_PVTMON_INTR_MASK_0r_t reg;
    uint32_t mask;

    ioerr += READ_TOP_PVTMON_INTR_MASK_0r(unit, &reg);
    mask = TOP_PVTMON_INTR_MASK_0r_GET(reg);
    if (enable) {
        mask |= (INTR_MASK_MIN << (chan * 2));
    } else {
        mask &= ~(INTR_MASK_MIN << (chan * 2));
    }

    TOP_PVTMON_INTR_MASK_0r_SET(reg, mask);
    ioerr += WRITE_TOP_PVTMON_INTR_MASK_0r(unit, reg);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}

static void
pvt_config(int unit)
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
        h_max_data[ix] = TOP_PVTMON_RESULT_0r_MIN_PVT_DATAf_GET(hist);
        h_min_data[ix] = TOP_PVTMON_RESULT_0r_MAX_PVT_DATAf_GET(hist);

        h_max_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, h_max_data[ix]);
        h_min_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, h_min_data[ix]);

        /* Read out interrupt threshold config. */
        TOP_PVTMON_INTR_THRESHOLDr_CLR(threshold);
        ioerr += READ_TOP_PVTMON_INTR_THRESHOLDr(unit, ix, &threshold);
        t_min_data[ix] = TOP_PVTMON_INTR_THRESHOLDr_MIN_THRESHOLDf_GET(threshold);
        t_max_data[ix] = TOP_PVTMON_INTR_THRESHOLDr_MAX_THRESHOLDf_GET(threshold);

        t_min_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, t_min_data[ix]);
        t_max_temp[ix] = pvt_temp_convert(PVT_EQUATION_1, t_max_data[ix]);

        /* Read out hardware reset threshold config. */
        ioerr += READ_TOP_PVTMON_HW_RST_THRESHOLDr(unit, ix, &hw_threshold);
        hw_threshold_d[ix] =
            TOP_PVTMON_HW_RST_THRESHOLDr_MIN_HW_RST_THRESHOLDf_GET(hw_threshold);
        hw_threshold_t[ix] = pvt_temp_convert(PVT_EQUATION_1, hw_threshold_d[ix]);
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
pvt_intr_status(int unit)
{
    int ioerr = 0;
    TOP_PVTMON_INTR_STATUS_0r_t reg;
    TOP_PVTMON_RESULT_1r_t result_1;
    TOP_PVTMON_INTR_THRESHOLDr_t threshold;
    int min = 0;
    uint32_t mask;
    int ix;
    bcmbd_temperature_t thermal;
    bcmbd_temperature_t c_temp, t_temp;
    bool overheating = false;
    bool print_config = false;

    ioerr += READ_TOP_PVTMON_INTR_STATUS_0r(unit, &reg);
    mask = TOP_PVTMON_INTR_STATUS_0r_GET(reg);
    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        /* AVS sensor is disabled for interrupt notification */
        min = (mask & (INTR_STATUS_MIN << (ix * 2))) >> (ix * 2 + 1);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "mask: 0x%x, s: %d, min: %d\n"), mask, ix, min));
        if (min == 1) {
            /* Read out sensor temperature. */
            TOP_PVTMON_RESULT_1r_CLR(result_1);
            ioerr += READ_TOP_PVTMON_RESULT_1r(unit, ix, &result_1);
            thermal = TOP_PVTMON_RESULT_1r_PVT_DATAf_GET(result_1);
            c_temp = pvt_temp_convert(PVT_EQUATION_1, thermal);

            /* Read out threshold config. */
            TOP_PVTMON_INTR_THRESHOLDr_CLR(threshold);
            ioerr += READ_TOP_PVTMON_INTR_THRESHOLDr(unit, ix, &threshold);
            thermal = TOP_PVTMON_INTR_THRESHOLDr_MIN_THRESHOLDf_GET(threshold);
            t_temp = pvt_temp_convert(PVT_EQUATION_1, thermal);

            if (c_temp > t_temp) {
                /* Temp readout is higher than min threshold, overheating */
                overheating = true;
                print_config = true;
            } else {
                overheating = false;
            }

            if (overheating) {
                /* Report per sensor message to console to notify user */
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "HMON overheating - Sensor %d: "
                                     "Temp: %.1f, Threshold Min: %.1f\n"),
                          ix, c_temp, t_temp));
            }
        }
    }

    if (print_config) {
        /* Print out all PVT configuration in case of overheating. */
        pvt_config(unit);
    }

    /* Write 1 to clear status bits to dessert interrupt */
    TOP_PVTMON_INTR_STATUS_0r_SET(reg, 0xFFFFFFFF);
    ioerr += WRITE_TOP_PVTMON_INTR_STATUS_0r(unit, reg);

    if (ioerr) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
    }
}

/*******************************************************************************
 * PVT driver
 */

static int
pvt_temperature_get(int unit,
                    size_t size,
                    bcmbd_pvt_equation_t equation,
                    bcmbd_pvt_temp_rec_t *readout,
                    size_t *num_rec)

{
    int ioerr = 0;
    TOP_PVTMON_RESULT_0r_t result_0;
    TOP_PVTMON_RESULT_1r_t result_1;
    AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_t avs_sensor;
    int ix, valid;
    bcmbd_temperature_t thermal;
    bcmbd_pvt_temp_rec_t *rptr_linear = NULL;
    bcmbd_pvt_temp_rec_t *rptr_sec_order = NULL;

    /* First API call to request number of sensors supported */
    if (size == 0) {
        *num_rec = PVT_SENSOR_CNT;
        return SHR_E_NONE;
    }

    /* Second API call to populate data record with chip readout */
    if (size > PVT_SENSOR_CNT){
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "HMON temperature get size (%d) out of bound.\n"),
                  (int)size));
        return SHR_E_PARAM;
    }

    if (equation == PVT_EQUATION_1) {
        rptr_linear = readout;
    } else if (equation == PVT_EQUATION_2) {
        rptr_linear = readout;
        rptr_sec_order = readout + PVT_SENSOR_CNT;
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "HMON temperature get equation (%d) not found.\n"),
                  (int)equation));
        return SHR_E_PARAM;
    }

    TOP_PVTMON_RESULT_0r_CLR(result_0);
    for (ix = 0; ix < (int) size; ix++) {
        if (ix == AVS_SENSOR) {
            ioerr += READ_AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr(unit, &avs_sensor);
            valid = AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_VALID_DATAf_GET(avs_sensor);
            thermal = AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_DATAf_GET(avs_sensor);
            if (valid) {
                /* Value of thermal read out - High temp, Low value */
                avs_temp[unit].current = thermal;

                /* Init local AVS sensor record value - lower means high temp
                 * data     offset  linear          Quadratic
                 * 1394     0       24.9            25.1
                 * 1183     0       75.0            75.1
                 * 1098     0       95.2            95.0
                 * 1055     0       105.4           105.0
                 */
                if (avs_temp[unit].max == 0) {
                    avs_temp[unit].max = thermal;
                }

                if (avs_temp[unit].min == 0) {
                    avs_temp[unit].min = thermal;
                }

                if (thermal > avs_temp[unit].max) {
                    avs_temp[unit].max = thermal;
                }

                if (thermal < avs_temp[unit].min) {
                    avs_temp[unit].min = thermal;
                }

                rptr_linear->current = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].current);
                rptr_linear->max = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].min);
                rptr_linear->min = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].max);

                if (equation == PVT_EQUATION_2) {
                    rptr_sec_order->current = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].current);
                    rptr_sec_order->max = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].min);
                    rptr_sec_order->min = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].max);
                }
            } else {
                /* Use local stored value. */
                rptr_linear->current = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].current);
                rptr_linear->max = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].min);
                rptr_linear->min = pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].max);

                if (equation == PVT_EQUATION_2) {
                    rptr_sec_order->current = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].current);
                    rptr_sec_order->max = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].min);
                    rptr_sec_order->min = pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].max);
                }
            }
        } else {
            ioerr += READ_TOP_PVTMON_RESULT_0r(unit, ix, &result_0);

            thermal = TOP_PVTMON_RESULT_0r_MIN_PVT_DATAf_GET(result_0);
            rptr_linear->max = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->max = pvt_temp_convert(PVT_EQUATION_2, thermal);
            }

            thermal = TOP_PVTMON_RESULT_0r_MAX_PVT_DATAf_GET(result_0);
            rptr_linear->min = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->min = pvt_temp_convert(PVT_EQUATION_2, thermal);
            }

            TOP_PVTMON_RESULT_1r_CLR(result_1);
            ioerr += READ_TOP_PVTMON_RESULT_1r(unit, ix, &result_1);

            thermal = TOP_PVTMON_RESULT_1r_PVT_DATAf_GET(result_1);
            rptr_linear->current = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->current = pvt_temp_convert(PVT_EQUATION_2, thermal);
            }
        }

        rptr_linear++;
        if (equation == PVT_EQUATION_2) {
            rptr_sec_order++;
        }
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static bcmbd_pvt_drv_t pvt_drv = {
    .temp_get = pvt_temperature_get,
};

/*******************************************************************************
 * PVT Interrupt driver
 */

static void
pvt_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;
    int ix;

    while (1) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        /* Output overheat message */
        pvt_intr_status(unit);

        /* Reset PVT hardware module to exclude false alarm */
        bcm56880_a0_pvt_init(unit);

        /* Enable individual pvt interrupt */
        for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
            pvt_intr_mask(unit, ix, 1);
        }

        /* Enable interrupt */
        bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);
    }
}

static void
pvt_intr_isr(int unit, uint32_t param)
{
    int ix;

    /* Disable interrupt */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PVTMON_INTR);

    /* Disable individual pvt interrupt */
    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        pvt_intr_mask(unit, ix, 0);
    }

    shr_thread_wake(pvt_thread_ctrl[unit]);
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
pvt_intr_func_set(int unit,
                  unsigned int intr_num,
                  bcmbd_intr_f intr_func,
                  uint32_t intr_param)
{
    /* Hook interrupt handler */
    bcmbd_cmicx_lp_intr_func_set(unit, PVTMON_INTR, pvt_intr_isr, 0);

    return SHR_E_NONE;
}

static void
pvt_intr_enable(int unit, unsigned int intr_num)
{
    int ix;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Enable PVTMON intr\n")));
    /* Enable PVTMON interrupt */
    bcmbd_cmicx_lp_intr_enable(unit, PVTMON_INTR);

    /* Enable individual pvt interrupt */
    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        pvt_intr_mask(unit, ix, 1);
    }
}

static void
pvt_intr_disable(int unit, unsigned int intr_num)
{
    int ix;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Disable PVTMON intr\n")));

    /* Disable interrupt */
    bcmbd_cmicx_lp_intr_disable_nosync(unit, PVTMON_INTR);

    /* Disable individual pvt interrupt */
    for (ix = 0; ix < (PVT_SENSOR_CNT - 1); ix++) {
        pvt_intr_mask(unit, ix, 0);
    }
}

static void
pvt_intr_clear(int unit, unsigned int intr_num)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Clear PVTMON intr\n")));

    pvt_intr_status(unit);
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

/*!
 * \brief Process Voltage Temperature driver init.
 *
 * \param[in] unit Unit number.
 *
 * \return converted value.
 */
int
bcm56880_a0_bd_pvt_intr_drv_init(int unit)
{
    int rv = SHR_E_NONE;
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    /* Show command handler hookup */
    rv = bcmbd_pvt_drv_init(unit, &pvt_drv);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "PVT driver init error\n")));
        (void)pvt_intr_cleanup(unit);

        return rv;
    }

    /* Interrupt handler hookup */
    rv = bcmbd_pvt_intr_drv_init(unit, &pvt_intr_drv);
    if (SHR_SUCCESS(rv)) {
        rv = pvt_intr_init(unit);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PVT intr enable error\n")));
            (void)pvt_intr_cleanup(unit);
        }
    }

    return rv;
}

/*!
 * \brief Process Voltage Temperature driver clean up.
 *
 * \param[in] unit Unit number.
 *
 * \return converted value.
 */
int
bcm56880_a0_bd_pvt_intr_drv_cleanup(int unit)
{
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (emul) {
        return SHR_E_NONE;
    } else {
        (void)bcmbd_pvt_drv_init(unit, NULL);
        (void)bcmbd_pvt_intr_drv_init(unit, NULL);
        return pvt_intr_cleanup(unit);
    }
}
