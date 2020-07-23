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

#define _SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C

#define _SOC_AVS_TH_VOLTAGE_GET_USE_I2C
/* #define _SOC_AVS_TH_VOLTAGE_GET_USE_PVTMON */

#define _SOC_AVS_TH_VOLTAGE_SET_USE_I2C
/* #define _SOC_AVS_TH_VOLTAGE_SET_USE_PVTMON - not yet implemented */

#include <soc/avs.h>

#if defined(_SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C) || defined(_SOC_AVS_TH_VOLTAGE_GET_USE_I2C) || defined(_SOC_AVS_TH_VOLTAGE_SET_USE_I2C)
#include <soc/i2c.h>
#endif



/*---------------------------------------------------------------------------*/
#if defined(_SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C) || defined(_SOC_AVS_TH_VOLTAGE_GET_USE_I2C) || defined(_SOC_AVS_TH_VOLTAGE_SET_USE_I2C)
typedef
struct th_avs_vconfig_params{
    uint8 mux;
    char *adc;
    int  chan;
    char *dac;
} th_avs_vparams_t;

STATIC char *_soc_th_avs_board_vctl_sel;

#define MUX_0 0
#define MUX_2 2
th_avs_vparams_t
th_adp4000_vs_config = {1 << MUX_0, I2C_ADC_0, I2C_ADC_CH0, "pwctrl2"};

th_avs_vparams_t
th_ltc388x_vs_config = {1 << MUX_2, I2C_LTC3882_55, I2C_BOTH_CH, I2C_LTC3882_55};
#endif

#if defined(_SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C) || defined(_SOC_AVS_TH_VOLTAGE_SET_USE_I2C)
static dac_calibrate_t
th_adp4000_dac_params = {0,"Core",-1,1,0,0,ADP4000_MIN_DACVAL,ADP4000_MAX_DACVAL-0x3,ADP4000_MID_DACVAL,1};
static dac_calibrate_t
th_ltc388x_dac_params = {0,"Core",-1,1,0,0,LTC388x_1_5V_DACVAL,LTC388x_0_6V_DACVAL,LTC388x_1V_DACVAL,1};
#endif

#if defined(_SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C)
STATIC soc_error_t
_soc_th_i2c_adp4000_voltage_calibrate(int unit)
{
    int adc, dac, rv, mux;
    i2c_adc_t  adc_data;
#ifdef COMPILER_HAS_DOUBLE
    double volts;
#else
    int volts;
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
                write(unit, mux, 0, &th_adp4000_vs_config.mux, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: write of lpt byte failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Open ADC device */
    if((adc = soc_i2c_devopen(unit, th_adp4000_vs_config.adc, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_adp4000_vs_config.adc, soc_errmsg(adc)));
        return SOC_E_FAIL;
    }

    /* Open the correct DAC */
    if ( (dac = soc_i2c_devopen(unit, th_adp4000_vs_config.dac, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_adp4000_vs_config.dac, soc_errmsg(dac)));
        return SOC_E_FAIL;
    }

    /* Set DAC calibration table */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SET_CALTAB, (void*)&th_adp4000_dac_params, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set DAC calibration table.\n")));
        return SOC_E_FAIL;
    }

    /* Set the DAC to the (digital) value which generates the minimum analog
       voltage at the ADC. */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SETDAC_MIN, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set min DAC Vout.\n")));
        return SOC_E_FAIL;
    }

    /* Read the resultant (minimum) voltage at the ADC. */
    if((rv = soc_i2c_device(unit, adc)->driver->ioctl(unit, adc,
                I2C_ADC_QUERY_CHANNEL,&adc_data, th_adp4000_vs_config.chan)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to read A/D min voltage.\n")));
        return SOC_E_FAIL;
    }

    volts = adc_data.val;

    /* Update DAC calibration table with this minimum voltage. */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_CALIBRATE_MIN, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to update min DAC Vout.\n")));
        return SOC_E_FAIL;
    }

    /* Set the DAC to the (digital) value which generates the maximum analog
       voltage at the ADC. */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SETDAC_MAX, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set max DAC Vout.\n")));
        return SOC_E_FAIL;
    }

    /* Read the resultant (maximum) voltage at the ADC. */
    if((rv = soc_i2c_device(unit, adc)->driver->ioctl(unit, adc,
                I2C_ADC_QUERY_CHANNEL, &adc_data, th_adp4000_vs_config.chan)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to read A/D max voltage.\n")));
        return SOC_E_FAIL;
    }

    volts = adc_data.val;

    /* Update DAC calibration table with this maximum voltage. */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_CALIBRATE_MAX, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to update max DAC Vout.\n")));
        return SOC_E_FAIL;
    }

    /* Very important, set the DAC back to nominal */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SETDAC_MID, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: resetting of DAC to nominal value failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* update the calibration table with the analog voltage step per DAC
       digital step. */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_CALIBRATE_STEP, &volts, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to calibrate DAC.\n")));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}
#endif /* _SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C */

#if defined(_SOC_AVS_TH_VOLTAGE_GET_USE_I2C)
STATIC soc_error_t
_soc_th_i2c_adp4000_voltage_get(int unit, uint32 *val)
{
    int adc, rv;
    i2c_adc_t  adc_data;

    /* Open I2C device */
    if((adc = soc_i2c_devopen(unit, th_adp4000_vs_config.adc, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_adp4000_vs_config.adc, soc_errmsg(adc)));
        return SOC_E_FAIL;
    }

    /* Read the voltage */
    if ((adc < 0) || (adc >= soc_i2c_device_count(unit))) {
        return SOC_E_PARAM;
    }

    if (soc_i2c_device(unit, adc)->driver == NULL) {
        return SOC_E_PARAM;
    }

    if((rv = soc_i2c_device(unit, adc)->driver->ioctl(unit, adc,
                I2C_ADC_QUERY_CHANNEL,&adc_data, th_adp4000_vs_config.chan)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to read A/D voltage.\n")));
        return SOC_E_FAIL;
    }

#ifdef COMPILER_HAS_DOUBLE
    *val = (adc_data.val * 10000);
#else
    *val = adc_data.val;
#endif
    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_th_i2c_ltc388x_voltage_get(int unit, uint32 *val)
{
    int adc, rv;
#ifdef COMPILER_HAS_DOUBLE
    double value=0;
#else
    int value=0;
#endif

    /* Open I2C device */
    if((adc = soc_i2c_devopen(unit, th_ltc388x_vs_config.adc, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_ltc388x_vs_config.adc, soc_errmsg(adc)));
        return SOC_E_FAIL;
    }

    /* Read the voltage */
    if((rv = soc_i2c_device(unit, adc)->driver->ioctl(unit, adc,
                I2C_LTC_IOC_READ_VOUT,&value, th_ltc388x_vs_config.chan)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to read VDD_Core's LTC device.\n")));
        return SOC_E_FAIL;
    }

#ifdef COMPILER_HAS_DOUBLE
    *val = (value * 10000);
#else
    *val = value;
#endif
    return SOC_E_NONE;
}

#endif /*_SOC_AVS_TH_VOLTAGE_GET_USE_I2C */

#if defined(_SOC_AVS_TH_VOLTAGE_SET_USE_I2C)
STATIC soc_error_t
_soc_th_i2c_adp4000_voltage_set(int unit, uint32 val)
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
                write(unit, mux, 0, &th_adp4000_vs_config.mux, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: write of lpt byte failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Open the correct DAC */
    if ( (dac = soc_i2c_devopen(unit, th_adp4000_vs_config.dac, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_adp4000_vs_config.dac, soc_errmsg(dac)));
        return SOC_E_FAIL;
    }

    /* Set DAC calibration table */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SET_CALTAB, (void*)&th_adp4000_dac_params, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set DAC calibration table.\n")));
        return SOC_E_FAIL;
    }

    /* Finally, set the actual voltage ... */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SET_VOUT, &value, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set DAC VDD_Core %d\n"), val));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_th_i2c_ltc388x_voltage_set(int unit, uint32 val)
{
    int rv, dac, mux, offset = 7;
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
    if ((mux = soc_i2c_devopen(unit, I2C_MUX_70, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s for mux selection:%s\n"),
                        I2C_MUX_3, soc_errmsg(mux)));
        return SOC_E_FAIL;
    }

    /* Set mux value so we can see the device when we try to open it */
    if((rv = soc_i2c_device(unit, mux)->driver->
                write(unit, mux, 0, &th_ltc388x_vs_config.mux, 1)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: write of lpt byte failed:%s\n"),
                        soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Open the correct DAC */
    if ( (dac = soc_i2c_devopen(unit, th_ltc388x_vs_config.dac, 0, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Could not open %s:%s\n"),
                        th_adp4000_vs_config.dac, soc_errmsg(dac)));
        return SOC_E_FAIL;
    }

    /* Set DAC calibration table */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_DAC_IOC_SET_CALTAB, (void*)&th_ltc388x_dac_params, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                        "Error: failed to set DAC calibration table.\n")));
        return SOC_E_FAIL;
    }

    /* Set LTC sense resistor */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_LTC_IOC_SET_CONFIG, &offset, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                    "Error: failed to configure max and min of device .\n")));
    }

    /* Finally, set the actual voltage ... */
    if ((rv = soc_i2c_device(unit, dac)->driver->ioctl(unit, dac,
                I2C_LTC_IOC_SET_VOUT, &value, 0) < 0)) {
        LOG_ERROR(BSL_LS_SOC_AVS,
            (BSL_META_U(unit,
                    "Error: failed to set VDD_Core's LTC device %d\n"), val));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}
#endif /*_SOC_AVS_TH_VOLTAGE_SET_USE_I2C */


/*---------------------------------------------------------------------------*/
soc_error_t
soc_th_avs_vcore_init(int unit)
{
#if defined(_SOC_AVS_TH_VOLTAGE_CALIBRATE_USE_I2C)
    _soc_th_avs_board_vctl_sel = soc_property_get_str(unit,
                                                      "th_avs_board_vctl_sel");
    if (_soc_th_avs_board_vctl_sel != NULL) {
        if ((sal_strcmp(_soc_th_avs_board_vctl_sel, "ADP4000") == 0)) {
            return _soc_th_i2c_adp4000_voltage_calibrate(unit);
        }
    }
#endif

   return SOC_E_NONE;
}

soc_error_t
soc_th_avs_vcore_deinit(int unit)
{
   return SOC_E_NONE;
}

soc_error_t
soc_th_avs_vcore_get(int unit, uint32 *voltage)
{
#if defined(_SOC_AVS_TH_VOLTAGE_GET_USE_I2C)
    if (_soc_th_avs_board_vctl_sel != NULL) {
        if ((sal_strcmp(_soc_th_avs_board_vctl_sel, "ADP4000") == 0)) {
            return _soc_th_i2c_adp4000_voltage_get(unit, voltage);
        } else if ((sal_strcmp(_soc_th_avs_board_vctl_sel, "LTC388X") == 0)) {
            return _soc_th_i2c_ltc388x_voltage_get(unit, voltage);
        }
    }
#endif

#if defined(_SOC_AVS_TH_VOLTAGE_GET_USE_PVTMON)
   return soc_avs_pvtmon_voltage_get(unit, voltage);
#endif

   return SOC_E_UNAVAIL;
}

soc_error_t
soc_th_avs_vcore_set(int unit, uint32 voltage)
{
#if defined(_SOC_AVS_TH_VOLTAGE_SET_USE_I2C)
    if (_soc_th_avs_board_vctl_sel != NULL) {
        if ((sal_strcmp(_soc_th_avs_board_vctl_sel, "ADP4000") == 0)) {
            return _soc_th_i2c_adp4000_voltage_set(unit, voltage);
        } else if ((sal_strcmp(_soc_th_avs_board_vctl_sel, "LTC388X") == 0)) {
            return _soc_th_i2c_ltc388x_voltage_set(unit, voltage);
        }
    }
#endif

   return SOC_E_UNAVAIL;
}

#endif /* INCLUDE_AVS */
