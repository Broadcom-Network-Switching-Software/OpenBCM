/*! \file bcm56996_a0_bd_pvt.c
 *
 * PVTMON thermal monitor driver.
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
#include <bcmbd/bcmbd_pvt_internal.h>
#include <bcmbd/bcmbd_pvt_intr.h>
#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmbd/chip/bcm56996_a0_pvt_intr.h>

#include "bcm56996_a0_drv.h"

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */
#define MAX_UNITS   BCMDRD_CONFIG_MAX_UNITS

#define HW_RESET_TEMPERATURE            115
#define SW_MIN_INTERRUPT_TEMPERATURE    105
#define SW_MAX_INTERRUPT_THRESHOLD      0x7FF

#define AVS_SENSOR_IDX  15
#define PVT_SENSOR_CNT  16


typedef struct pvt_event_s {
    uint32_t mon_idx;
    bcmbd_pvt_event_t evt_type;
    bool pending;
} pvt_event_t;


/*******************************************************************************
 * Local data
 */

/* Per-unit PVT data conversion equation  */
static bcmbd_pvt_equation_t pvt_equation[MAX_UNITS];

/* Per-unit PVT pending HW reset event */
static pvt_event_t pvt_event[MAX_UNITS];

/* Per-unit PVT event handler */
static bcmbd_pvt_event_cb_f pvt_event_cb[MAX_UNITS];

/*
 * AVS sensor doesn't have hist min and max control
 * Keep a software copy here
 */
static bcmbd_avs_t avs_temp[MAX_UNITS];

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
        LOG_ERROR(BSL_LOG_MODULE,
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
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "HMON temperature get equation (%d) not found.\n"),
                  (int)equation));
        return SHR_E_PARAM;
    }

    TOP_PVTMON_RESULT_0r_CLR(result_0);
    for (ix = 0; ix < (int) size; ix++) {
        if (ix == AVS_SENSOR_IDX) {
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

                if (thermal < avs_temp[unit].max) {
                    avs_temp[unit].max = thermal;
                }

                if (thermal > avs_temp[unit].min) {
                    avs_temp[unit].min = thermal;
                }

                rptr_linear->current =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].current);
                rptr_linear->max =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].min);
                rptr_linear->min =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].max);

                if (equation == PVT_EQUATION_2) {
                    rptr_sec_order->current =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].current);
                    rptr_sec_order->max =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].min);
                    rptr_sec_order->min =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].max);
                }
            } else {
                /* Use local stored value. */
                rptr_linear->current =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].current);
                rptr_linear->max =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].min);
                rptr_linear->min =
                    pvt_temp_convert(PVT_EQUATION_1, avs_temp[unit].max);

                if (equation == PVT_EQUATION_2) {
                    rptr_sec_order->current =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].current);
                    rptr_sec_order->max =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].min);
                    rptr_sec_order->min =
                        pvt_temp_convert(PVT_EQUATION_2, avs_temp[unit].max);
                }
            }
        } else {
            ioerr += READ_TOP_PVTMON_RESULT_0r(unit, ix, &result_0);

            thermal = TOP_PVTMON_RESULT_0r_MAX_PVT_DATAf_GET(result_0);
            rptr_linear->max = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->max = pvt_temp_convert(PVT_EQUATION_2, thermal);
            }

            thermal = TOP_PVTMON_RESULT_0r_MIN_PVT_DATAf_GET(result_0);
            rptr_linear->min = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->min = pvt_temp_convert(PVT_EQUATION_2, thermal);
            }

            TOP_PVTMON_RESULT_1r_CLR(result_1);
            ioerr += READ_TOP_PVTMON_RESULT_1r(unit, ix, &result_1);

            thermal = TOP_PVTMON_RESULT_1r_PVT_DATAf_GET(result_1);
            rptr_linear->current = pvt_temp_convert(PVT_EQUATION_1, thermal);
            if (equation == PVT_EQUATION_2) {
                rptr_sec_order->current =
                    pvt_temp_convert(PVT_EQUATION_2, thermal);
            }
        }

        rptr_linear++;
        if (equation == PVT_EQUATION_2) {
            rptr_sec_order++;
        }
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}


static int
pvt_event_func_set(int unit, bcmbd_pvt_event_cb_f func)
{
    pvt_event_cb[unit] = func;

    /* HW reset event detected before event callback func register */
    if (func && pvt_event[unit].pending) {
        func(unit,
             pvt_event[unit].evt_type,
             pvt_event[unit].mon_idx,
             0);
        pvt_event[unit].pending = false;
    }

    return SHR_E_NONE;
}


static int
pvt_mon_reset(int unit, unsigned int mon_idx)
{
    TOP_TMON_CHANNELS_CTRL_0r_t data_reset;
    TOP_PVTMON_CTRL_0r_t mon_reset;
    int ioerr = 0;
    uint32_t bits;

    if (mon_idx != AVS_SENSOR_IDX) {
        /* Reset PVTMON */
        ioerr += READ_TOP_PVTMON_CTRL_0r(unit, mon_idx, &mon_reset);
        TOP_PVTMON_CTRL_0r_RESET_Nf_SET(mon_reset, 0);
        ioerr += WRITE_TOP_PVTMON_CTRL_0r(unit, mon_idx, mon_reset);
        sal_usleep(200);
    }

    /* Reset history min and max stick data registers */
    ioerr += READ_TOP_TMON_CHANNELS_CTRL_0r(unit, &data_reset);
    bits = TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MAX_RST_Lf_GET(data_reset);
    bits &= ~LSHIFT32(1, mon_idx);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MAX_RST_Lf_SET(data_reset, bits);

    bits = TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MIN_RST_Lf_GET(data_reset);
    bits &= ~LSHIFT32(1, mon_idx);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MIN_RST_Lf_SET(data_reset, bits);

    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_0r(unit, data_reset);
    sal_usleep(200);

    if (mon_idx != AVS_SENSOR_IDX) {
        /* PVTMON out of reset */
        ioerr += READ_TOP_PVTMON_CTRL_0r(unit, mon_idx, &mon_reset);
        TOP_PVTMON_CTRL_0r_RESET_Nf_SET(mon_reset, 1);
        ioerr += WRITE_TOP_PVTMON_CTRL_0r(unit, mon_idx, mon_reset);
        sal_usleep(200);
    }

    /* Stick data registers out of reset */
    ioerr += READ_TOP_TMON_CHANNELS_CTRL_0r(unit, &data_reset);

    bits = TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MAX_RST_Lf_GET(data_reset);
    bits |= LSHIFT32(1, mon_idx);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MAX_RST_Lf_SET(data_reset, bits);

    bits = TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MIN_RST_Lf_GET(data_reset);
    bits |= LSHIFT32(1, mon_idx);
    TOP_TMON_CHANNELS_CTRL_0r_TMON_CHANNELS_MIN_RST_Lf_SET(data_reset, bits);

    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_0r(unit, data_reset);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_hw_reset_enable(int unit, unsigned int mon_idx, int enable)
{
    TOP_TMON_CHANNELS_CTRL_1r_t chan_ctrl;
    int ioerr = 0;
    uint32_t hw;

    TOP_TMON_CHANNELS_CTRL_1r_CLR(chan_ctrl);
    ioerr += READ_TOP_TMON_CHANNELS_CTRL_1r(unit, &chan_ctrl);
    hw = TOP_TMON_CHANNELS_CTRL_1r_TMON_HW_RST_HIGHTEMP_CTRL_ENf_GET(chan_ctrl);

    if (enable) {
        hw |= LSHIFT32(1, mon_idx);
    } else {
        hw &= ~LSHIFT32(1, mon_idx);
    }

    TOP_TMON_CHANNELS_CTRL_1r_TMON_HW_RST_HIGHTEMP_CTRL_ENf_SET(chan_ctrl, hw);
    ioerr += WRITE_TOP_TMON_CHANNELS_CTRL_1r(unit, chan_ctrl);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_hw_reset_threshold_set(int unit, unsigned int mon_idx, int temp)
{
    TOP_PVTMON_HW_RST_THRESHOLDr_t threshold;
    uint32_t temp_data;
    int ioerr = 0;

    /* Temp = -0.23751*data + 356.01
     * data = (356.01 - temp) / 0.23751
     *      = (356010 - (temp * 1000)) / 238
     */
    temp_data = (356010 - (temp * 1000)) / 238;

    TOP_PVTMON_HW_RST_THRESHOLDr_CLR(threshold);
    TOP_PVTMON_HW_RST_THRESHOLDr_MIN_HW_RST_THRESHOLDf_SET(threshold,
                                                           temp_data);
    ioerr += WRITE_TOP_PVTMON_HW_RST_THRESHOLDr(unit, mon_idx, threshold);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_sw_intr_threshold_set(int unit, unsigned int mon_idx, int temp)
{
    TOP_PVTMON_INTR_THRESHOLDr_t threshold;
    uint32_t temp_data;
    int ioerr = 0;

    TOP_PVTMON_INTR_THRESHOLDr_CLR(threshold);

     /* Temp = -0.23751*data + 356.01
      * data = (356.01 - temp)/0.23751
             = (356010 - (temp * 1000)) / 238
      */
    /* Max threshold (low temp) default value per chip specified. */
    TOP_PVTMON_INTR_THRESHOLDr_MAX_THRESHOLDf_SET(threshold,
                                                  SW_MAX_INTERRUPT_THRESHOLD);

    temp_data = (356010 - (temp * 1000)) / 238;
    TOP_PVTMON_INTR_THRESHOLDr_MIN_THRESHOLDf_SET(threshold, temp_data);
    ioerr += WRITE_TOP_PVTMON_INTR_THRESHOLDr(unit, mon_idx, threshold);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
pvt_result_get(int unit, int idx, uint32_t *data)
{
    TOP_PVTMON_RESULT_1r_t result_1;
    AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_t avs_sensor;
    int ioerr = 0;
    int valid;

    if (idx == AVS_SENSOR_IDX) {
        ioerr += READ_AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr(unit, &avs_sensor);
        valid = AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_VALID_DATAf_GET(avs_sensor);
        if (valid) {
            *data = AVS_PVT_MAIN_THERMAL_SENSOR_STATUSr_DATAf_GET(avs_sensor);
        }
    } else {
        ioerr += READ_TOP_PVTMON_RESULT_1r(unit, idx, &result_1);
        *data = TOP_PVTMON_RESULT_1r_PVT_DATAf_GET(result_1);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}


static void
pvt_intr_handler(int unit, uint32_t intr_param)
{
    uint32_t intr_num;
    uint32_t data = 0;
    uint32_t mon_idx;
    bcmbd_pvt_event_cb_f func;
    bcmbd_temperature_t temp;
    int rv = SHR_E_NONE;

    intr_num = intr_param;
    mon_idx = intr_num >> 1;

    rv = pvt_result_get(unit, mon_idx, &data);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON %d get result error\n"),
                   mon_idx));
        return;
    }

    temp = pvt_temp_convert(pvt_equation[unit], data);
    func = pvt_event_cb[unit];
    if (func) {
        func(unit, PVT_EVENT_HIGH_TEMP, mon_idx, (uint32_t)temp);
    }

    bcmbd_pvt_intr_clear(unit, intr_num);
}


static int
pvt_init(int unit)
{
    TOP_PVTMON_HW_RST_STATUSr_t rst_stat;
    TOP_MISC_CONTROL_1r_t misc_ctrl;
    uint32_t chan;
    int idx;
    int reset = 0;
    int ioerr = 0;
    int rv;
    int intr;

    /* Default equation */
    pvt_equation[unit] = PVT_EQUATION_1;
    sal_memset(&pvt_event[unit], 0, sizeof(pvt_event[unit]));
    pvt_event_cb[unit] = NULL;

    /* Check if overheat happened from last boot */
    TOP_PVTMON_HW_RST_STATUSr_CLR(rst_stat);
    ioerr += READ_TOP_PVTMON_HW_RST_STATUSr(unit, &rst_stat);
    chan = TOP_PVTMON_HW_RST_STATUSr_MIN_HW_RST_STATUSf_GET(rst_stat);
    for (idx = 0; idx < PVT_SENSOR_CNT; idx++) {
        if (chan & (1 << idx)) {
            reset = 1;
            /* record HW reset event */
            pvt_event[unit].mon_idx = idx;
            pvt_event[unit].evt_type = PVT_EVENT_HW_RESET;
            pvt_event[unit].pending = true;
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "HMON Sensor %d experienced overheat reset\n"),
                      idx));
        }
    }

    /* Now clear the sticky bit (W1TC) */
    if (reset) {
        TOP_MISC_CONTROL_1r_CLR(misc_ctrl);
        ioerr += READ_TOP_MISC_CONTROL_1r(unit, &misc_ctrl);
        TOP_MISC_CONTROL_1r_PVTMON_HIGHTEMP_STAT_CLEARf_SET(misc_ctrl, 1);
        ioerr += WRITE_TOP_MISC_CONTROL_1r(unit, misc_ctrl);
    }

    if (SHR_FAILURE(ioerr)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "PVTMON I/O error\n")));
        return SHR_E_IO;
    }

    for (idx = 0; idx < PVT_SENSOR_CNT; idx++) {
        /* Reset channels from fresh start */
        rv = pvt_mon_reset(unit, idx);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON %d reset failed\n"),
                       idx));
            return rv;
        }

        /* Set hardware reset threshold */
        rv = pvt_hw_reset_threshold_set(unit, idx, HW_RESET_TEMPERATURE);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON %d set hw reset threshold failed\n"),
                       idx));
            return rv;
        }

        /* Turn on hardware protection. */
        rv = pvt_hw_reset_enable(unit, idx, 1);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON %d enable hw reset failed\n"),
                       idx));
            return rv;
        }

        /* Set software interrupt threshold */
        rv = pvt_sw_intr_threshold_set(unit, idx, SW_MIN_INTERRUPT_TEMPERATURE);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON %d set sw intr threshold failed\n"),
                       idx));
            return rv;
        }
    }

    for (intr = PVT_INTR_BASE; intr < MAX_PVT_INTR; intr++) {
        /* Register interrupt handler */
        rv = bcmbd_pvt_intr_func_set(unit, intr, pvt_intr_handler, intr);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON intr %d set handler failed\n"),
                       intr));
            return rv;
        }

        /* Enable interrupt */
        rv = bcmbd_pvt_intr_enable(unit, intr);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "PVTMON intr %d enable failed\n"),
                       intr));
            return rv;
        }
    }

    return SHR_E_NONE;
}

static int
pvt_cleanup(int unit)
{
    return SHR_E_NONE;
}


static bcmbd_pvt_drv_t pvt_drv = {
    .event_func_set = pvt_event_func_set,
    .temp_get = pvt_temperature_get,
};

/*******************************************************************************
 * Public functions
 */

int
bcm56996_a0_bd_pvt_drv_init(int unit)
{
    int rv = SHR_E_NONE;
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    rv = bcmbd_pvt_drv_init(unit, &pvt_drv);
    if (SHR_SUCCESS(rv)) {
        rv = pvt_init(unit);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "PVT driver init error\n")));
            (void)pvt_cleanup(unit);
        }
    }

    return rv;
}

int
bcm56996_a0_bd_pvt_drv_cleanup(int unit)
{
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    if (emul) {
        return SHR_E_NONE;
    } else {
        (void)bcmbd_pvt_drv_init(unit, NULL);
        return pvt_cleanup(unit);
    }
}
