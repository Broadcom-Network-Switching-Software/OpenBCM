/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs_ext_vcore_ctl.c
 */

#include <shared/bsl.h>

#ifdef INCLUDE_AVS

#define _SOC_AVS_TH3_VOLTAGE_GET_USE_I2C
/* #define _SOC_AVS_TH3_VOLTAGE_GET_USE_PVTMON */

#define _SOC_AVS_TH3_VOLTAGE_SET_USE_I2C
/* #define _SOC_AVS_TH3_VOLTAGE_SET_USE_PVTMON - not yet implemented */

#include <soc/avs.h>

#if defined(_SOC_AVS_TH3_VOLTAGE_GET_USE_I2C) || defined(_SOC_AVS_TH3_VOLTAGE_SET_USE_I2C)
#include <soc/i2c.h>

typedef
struct th3_avs_vconfig_params {
    uint8 mux;
    char *adc;
    int  chan;
    char *dac;
} th3_avs_vparams_t;

STATIC char *_soc_tomahawk3_avs_board_vctl_sel;

#define MUX_0 0
#define MUX_2 2
#define MUX_3 3

th3_avs_vparams_t th3_isl68127_vs_config = {1 << MUX_3, I2C_ISL68127_60, I2C_ISL68127_CH1, I2C_ISL68127_60};

#endif

#if defined(_SOC_AVS_TH3_VOLTAGE_GET_USE_I2C)

STATIC soc_error_t
_soc_tomahawk3_i2c_isl68127_voltage_get(int unit, uint32 *val)
{
    int rv, adc, mux;
#ifdef COMPILER_HAS_DOUBLE
    double value;
#else
    int value;
#endif

    /* Going to get a voltage (need ADC), and the ADC is MUXed */
    /* Open MUX (lpt0) device */
    if ((mux = soc_i2c_devopen(unit, I2C_MUX_3, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s for mux selection:%s\n"),
                        I2C_MUX_3, soc_errmsg(mux)));
        return SOC_E_FAIL;
    }

    /* Set mux value so we can see the device when we try to open it */
    if((rv = soc_i2c_device(unit, mux)->driver->
                write(unit, mux, 0, &th3_isl68127_vs_config.mux, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: write of lpt byte failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Open the correct ADC */
    if ( (adc = soc_i2c_devopen(unit, th3_isl68127_vs_config.adc, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th3_isl68127_vs_config.adc, soc_errmsg(adc)));
        return SOC_E_FAIL;
    }

    /* Read the voltage */
    if((rv = soc_i2c_device(unit, adc)->driver->ioctl(unit, adc,
                PMBUS_IOC_READ_VOUT,&value, th3_isl68127_vs_config.chan)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to read VDD_Core's ISL device.\n")));
        return SOC_E_FAIL;
    }


#ifdef COMPILER_HAS_DOUBLE
    *val = (value * 10000);
#else
    *val = value;
#endif

    return SOC_E_NONE;
}

#endif /*_SOC_AVS_TH3_VOLTAGE_GET_USE_I2C */


#if defined(_SOC_AVS_TH3_VOLTAGE_SET_USE_I2C)

STATIC soc_error_t
_soc_tomahawk3_i2c_isl68127_voltage_set(int unit, uint32 val)
{
    int rv, dac, mux;
#ifdef COMPILER_HAS_DOUBLE
    double value = (double) val;
    value = value/10000;
#else
    int value;
    /* value = _shr_atof_exp10(val, 2); */
    value = val * 100; /* RHS is in 0.1mV units, we want LHS in uV */
#endif

    /* Going to set a voltage (need DAC), and the DAC is MUXed */
    /* Open MUX (lpt0) device */
    if ((mux = soc_i2c_devopen(unit, I2C_MUX_3, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s for mux selection:%s\n"),
                        I2C_MUX_3, soc_errmsg(mux)));
        return SOC_E_FAIL;
    }

    /* Set mux value so we can see the device when we try to open it */
    if((rv = soc_i2c_device(unit, mux)->driver->
                write(unit, mux, 0, &th3_isl68127_vs_config.mux, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: write of lpt byte failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Open the correct DAC */
    if ( (dac = soc_i2c_devopen(unit, th3_isl68127_vs_config.dac, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th3_isl68127_vs_config.dac, soc_errmsg(dac)));
        return SOC_E_FAIL;
    }

    /* Finally, set the actual voltage ... */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                PMBUS_IOC_SET_VOUT, &value, th3_isl68127_vs_config.chan) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                    "Error: failed to set VDD_Core's voltage regulator device %d\n"), val));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}
#endif /*_SOC_AVS_TH3_VOLTAGE_SET_USE_I2C */

soc_error_t
soc_tomahawk3_avs_vcore_init(int unit)
{
    _soc_tomahawk3_avs_board_vctl_sel = soc_property_get_str(unit,
                                                      "th_avs_board_vctl_sel");
   return SOC_E_NONE;
}

soc_error_t
soc_tomahawk3_avs_vcore_deinit(int unit)
{
   return SOC_E_NONE;
}

soc_error_t
soc_tomahawk3_avs_vcore_get(int unit, uint32 *voltage)
{
#if defined(_SOC_AVS_TH3_VOLTAGE_GET_USE_I2C)
    if (_soc_tomahawk3_avs_board_vctl_sel != NULL) {
        if ((sal_strcmp(_soc_tomahawk3_avs_board_vctl_sel, "ISL68127") == 0)) {
            return _soc_tomahawk3_i2c_isl68127_voltage_get(unit, voltage);
        }
    }
#endif

   return SOC_E_UNAVAIL;
}

soc_error_t
soc_tomahawk3_avs_vcore_set(int unit, uint32 voltage)
{
#if defined(_SOC_AVS_TH3_VOLTAGE_SET_USE_I2C)
    if (_soc_tomahawk3_avs_board_vctl_sel != NULL) {
        if ((sal_strcmp(_soc_tomahawk3_avs_board_vctl_sel, "ISL68127") == 0)) {
            return _soc_tomahawk3_i2c_isl68127_voltage_set(unit, voltage);
        }
    }
#endif

   return SOC_E_UNAVAIL;
}

#endif /* INCLUDE_AVS */
