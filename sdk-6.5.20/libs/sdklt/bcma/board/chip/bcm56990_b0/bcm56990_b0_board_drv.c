/*! \file bcm56990_b0_board_drv.c
 *
 * Chip functions for BCMA board driver attach.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_i2c.h>
#include <bcmbd/bcmbd_otp.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd_config.h>
#include <bcma/board/bcma_board_drv.h>
#include <bcma/i2c/bcma_i2ccmd_op.h>
#include <bcma/i2c/bcma_i2ccmd_bb.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_BOARD

/* Name of voltage source to be programmed with OTP voltage value */
#define VRM_NAME "Core"

/* Voltage granularity of AVS voltage value from OTP in millivolts(mV) */
#define VOLTAGE_STEPPING 6.25

/* Upper and lower votage ranges in millivolts. Values provided by
 * h/w engg
 */
#define VOLTAGE_RANGE_UPPER 800
#define VOLTAGE_RANGE_LOWER 740

/* Per VR11.1 specification and CHIP_CONFIG_OTP description of AVS_STATUS,
 * decimal count 98 corresponds to 1 Volt. After 98, each higher count
 * reduces the voltage value by VOLTAGE_STEPPING value. Example, value 98
 * corresponds to 1000mV, 102 corresponds to 1-(102-98)*6.25 = 975mV, 106
 * corresponds to 1-(106-98)*6.25= 950mV and so on
 */
#define ONE_VOLT_COUNT 98

static int avs_initialized = FALSE;

static bb_i2c_device_t bcma_board_voltage_profile[BCMDRD_CONFIG_MAX_UNITS];

/* Convert value read from OTP to voltage. Perform a check if the value is
 * within operating range
 */
static int bcma_otp_voltage_convert(uint32_t otp_val, double *voltage)
{
    if (otp_val >= ONE_VOLT_COUNT) {
        *voltage = (double)1000.0 -
                   (double)(otp_val - ONE_VOLT_COUNT) *
                   (double)VOLTAGE_STEPPING;
    } else {
        *voltage = (double)1000.0 +
                   (double)(ONE_VOLT_COUNT - otp_val) *
                   (double)VOLTAGE_STEPPING;
    }

    if ((*voltage > (double)VOLTAGE_RANGE_UPPER) ||
        (*voltage < (double)VOLTAGE_RANGE_LOWER)) {
        return SHR_E_CONFIG;
    }

    /* Convert value in to volts (because lower layer driver converts the
     * value back to mV)
     */
    *voltage = *voltage / (double)1000.0;

    return SHR_E_NONE;
}

static int
bcm56990_b0_board_avs_voltage_set(int unit, uint32_t otp_voltage_val)
{
    char *regulator_name;
    int rv = SHR_E_NONE;
    double voltage;

    if (bcma_i2c_device_drv[unit] &&
        bcma_i2c_device_drv[unit]->voltage_dev_get) {
        /* Derive board specific configuration */
        bcma_i2c_device_drv[unit]->voltage_dev_get(unit,
                                   &bcma_board_voltage_profile[unit]);
    } else {
        return SHR_E_FAIL;
    }

    rv = bcma_otp_voltage_convert(otp_voltage_val, &voltage);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                     "Error %d: AVS voltage %2.3f out of range\n"),
                     rv, voltage / (double)1000.0));
        return rv;
    }

    regulator_name = VRM_NAME;
    rv = bcma_bb_set_rail_voltage(unit,
                                  &bcma_board_voltage_profile[unit],
                                  regulator_name,
                                  &voltage);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                      "Error %d: AVS voltage could not be set\n"),
                      rv));
    }

    return rv;
}

static int
bcm56990_b0_board_avs_configure(int unit)
{
    uint32_t voltage = 0;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);

    /* On simulation platform, skip voltage programming */
    if (!bcmdrd_feature_is_real_hw(unit)) {
        return SHR_E_NONE;
    }

    /* Program AVS value only once, during system initialization */
    if (avs_initialized == FALSE) {
        /* Read AVS value from OTP */
        SHR_IF_ERR_EXIT
            (bcmbd_otp_param_get(unit, BCMBD_OTP_PRM_AVS_VOLTAGE, &voltage));

        /* Initiallize I2C */
        SHR_IF_ERR_EXIT
            (bcmbd_i2c_init(unit));

        if (!bcma_i2c_device_drv[unit]) {
            SHR_IF_ERR_EXIT
                (bcma_i2c_board_drv_init(unit));
        }

        /* Set Core voltage through I2C interface */
        SHR_IF_ERR_EXIT
            (bcm56990_b0_board_avs_voltage_set(unit, voltage));

        pb = shr_pb_create();
        shr_pb_printf(pb, "Unit %d: %s: AVS programmed\n", unit,
                      bcmdrd_dev_name(unit));
        shr_pb_destroy(pb);

        avs_initialized = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_board_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    /* Program AVS value obtained from OTP */
    SHR_IF_ERR_EXIT
        (bcm56990_b0_board_avs_configure(unit));

exit:
    SHR_FUNC_EXIT();
}

static bcma_board_drv_t bcm56990_b0_board_drv = {
    .board_init = bcm56990_b0_board_init
};

bcma_board_drv_t *
bcm56990_b0_board_drv_get(int unit)
{
    return &bcm56990_b0_board_drv;
}

