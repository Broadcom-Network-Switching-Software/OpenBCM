/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.c
 * Purpose:
 * Requires:
 */
 
#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/i2c.h>
#include <soc/saber2.h>
#include <soc/drv.h>
#include <soc/error.h>

#include <bcm/error.h>

#define SOC_AVS_SABER2_NUMBER_OF_CENTRALS 36
#define SOC_AVS_SABER2_CENTRAL_XBMP0 0x0
#define SOC_AVS_SABER2_CENTRAL_XBMP1 0xFFFFFFF0
#define SOC_AVS_SABER2_FIRST_CENTRAL 0

#define SOC_AVS_SABER2_NUMBER_OF_REMOTES 20
#define SOC_AVS_SABER2_REMOTE_XBMP0 0x0
#define SOC_AVS_SABER2_FIRST_REMOTE 0

#define SOC_AVS_SABER2_VMIN_AVS SOC_AVS_DEFAULT_VMIN_AVS
#define SOC_AVS_SABER2_VMAX_AVS SOC_AVS_DEFAULT_VMAX_AVS
#define SOC_AVS_SABER2_VMARGIN_HIGH SOC_AVS_DEFAULT_VMARGIN_HIGH
#define SOC_AVS_SABER2_VMARGIN_LOW SOC_AVS_DEFAULT_VMARGIN_LOW

static soc_avs_reg_info_t _sb2_osc_cen_init_list[]={
/* AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r */
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_0r,CEN_ROSC_ENABLE_DEFAULTf},
{AVS_REG_HW_MNTR_ENABLE_DEFAULT_CEN_ROSC_1r,CEN_ROSC_ENABLE_DEFAULTf},
{-1,-1},
};

static soc_avs_reg_info_t _sb2_osc_cen_thr_en_list[]={
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_1_CEN_ROSC_THRESHOLD1_EN_1r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_0r,IRQ_ENf},
{AVS_REG_ROSC_THRESHOLD_2_CEN_ROSC_THRESHOLD2_EN_1r,IRQ_ENf},
{-1,-1},
};

/* reset osc measurement */
static soc_avs_reg_info_t _sb2_osc_seq_reset_list[]={
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_0r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_CEN_ROSC_1r,M_INIT_CEN_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_0r,M_INIT_RMT_ROSCf},
{AVS_REG_HW_MNTR_MEASUREMENTS_INIT_RMT_ROSC_1r,M_INIT_RMT_ROSCf},
{-1,-1}
};

#ifndef SB2_AVS_I2C 
int
_soc_saber2_init_pvt(int unit) {
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON0_MAX_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON0_MIN_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON0_MAX_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON0_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON1_MAX_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));

    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON1_MIN_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));

    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON1_MAX_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));

    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval, THERMAL_PVTMON1_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));

    return SOC_E_NONE;
}
#endif

#ifdef SB2_AVS_I2C
static int dac[SOC_MAX_NUM_DEVICES], adc[SOC_MAX_NUM_DEVICES];

static dac_calibrate_t dac_params[] = {
    {0, "Core", -1, 1, 0, 0,
    LTC3880_0_6V_MIN_DACVAL,
    LTC3880_0_6V_MAX_DACVAL,
    LTC3880_0_6V_MID_DACVAL,1},
};


STATIC int
_soc_saber2_avs_i2c_voltage_set(int unit, uint32 val)
{
    int rv = SOC_E_NONE;
#ifdef COMPILER_HAS_DOUBLE
    double sb2_i2c_index = 6; /* Saber2 index in i2c device drivers */
    double volts = (double)val;
    volts = volts/10000;
#else
    int volts = val * 100; /* 0.1mv to uV */
    int sb2_i2c_index = 6; /* Saber2 index in i2c device drivers */
#endif

    /* Calulating voltage based on feedback ratio. */
    /* For 1V output, feedback voltage is 0.549 volts */
    volts = volts * 549 / 1000;

    if ((rv = soc_i2c_device(unit, dac[unit])->driver->ioctl(unit, dac[unit],
                I2C_LTC_IOC_SET_CONFIG, &sb2_i2c_index, 0) < 0)){
        LOG_ERROR(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Failed to set configure max and min\n")));
    }

    if ((rv = soc_i2c_device(unit, dac[unit])->driver->ioctl(unit, dac[unit],
                I2C_DAC_IOC_SET_CALTAB, (void*)dac_params, 1) < 0)){
        LOG_ERROR(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Failed to set DAC calibration\n")));
    }

    if ((rv = soc_i2c_device(unit, dac[unit])->driver->ioctl(unit, dac[unit],
                I2C_LTC_IOC_SET_VOUT, &volts, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Failed to set Voltage %f\n"), volts));
    }

    return rv;
}

STATIC int
_soc_saber2_avs_i2c_voltage_get(int unit, uint32 *val)
{
    int rv = SOC_E_NONE;
    
#ifdef COMPILER_HAS_DOUBLE
    double volts =  0;
#else
    int volts = 0;
#endif

    if ((rv = soc_i2c_device(unit, dac[unit])->driver->ioctl(unit, adc[unit],
                I2C_LTC_IOC_READ_VOUT, &volts, 0)) < 0) {
        LOG_ERROR(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Failed to get Voltage\n")));
    }
    
    /* Calulating voltage based on feedback ratio. */
    /* For 1V output, feedback voltage is 0.549 volts */
    volts = volts * 1000 / 549;

#ifdef COMPILER_HAS_DOUBLE
    *val = (float)volts * 10000;
#else
    *val = adc_data.val;
#endif

    return rv;
}


STATIC int sb2_dac_devs_init (int unit, char *dev_name)
{
    int rv = 0;
    int flags = 0;
    uint8 data;
    int mux;

    if ( (mux = soc_i2c_devopen(unit, dev_name, flags, 0)) < 0) {
        cli_out("Could not open %s for mux selection:%s\n",
                dev_name?dev_name:"MUX_DEV", bcm_errmsg(mux));
        return SOC_E_FAIL;
    }

    data = 1 << 7;
    if( (rv = soc_i2c_device(unit, mux)->driver->
                write(unit, mux, 0,
                            &data, 1)) < 0) {
        cli_out("Error: write of mux device byte failed:%s\n",
                bcm_errmsg(rv));
        return SOC_E_FAIL;
    }

    /* get dac diver's device id */
    if ((dac[unit] = soc_i2c_devopen(unit, I2C_LTC3880_41,
                            flags, 0)) < 0) {
        cli_out("Could not open %s:%s\n",
                I2C_LTC3880_41, bcm_errmsg(dac[unit]));
        return SOC_E_FAIL;
    }
    soc_i2c_device(unit, dac[unit])->driver->load(unit,dac[unit],NULL,0);

    if ((adc[unit] = soc_i2c_devopen(unit, I2C_LTC3880_41,
                            flags, 0)) < 0) {
        cli_out("Could not open %s:%s\n",
                I2C_LTC3880_41, bcm_errmsg(dac[unit]));
       return BCM_E_FAIL;
    }
    soc_i2c_device(unit, dac[unit])->driver->load(unit,adc[unit],NULL,0);

    return SOC_E_NONE;
}
#endif

int
_soc_saber2_avs_voltage_get(int unit, uint32 *val)
{
#ifdef SB2_AVS_I2C
    return _soc_saber2_avs_i2c_voltage_get(unit, val);
#else
    return soc_avs_pvtmon_voltage_get(unit, val);
#endif
}
    
int
_soc_saber2_avs_voltage_set(int unit, uint32 val)
{
#ifdef SB2_AVS_I2C
    return _soc_saber2_avs_i2c_voltage_set(unit, val);
#else
    return soc_avs_pvtmon_voltage_set(unit, val);
#endif
}
    
int
_soc_saber2_avs_info_ioctl(int unit, soc_avs_ioctl_t opcode, void* data, int len)
{
    int rv = SOC_E_NONE;

#ifndef SB2_AVS_I2C
    uint32 value;
    float v_dac;
    float v_bg[8] = {1.205, 1.200, 1.195, 1.191, 1.211, 1.215, 1.220, 1.226};
    uint32  bg_val, rval;
    float dac_val_temp, set_volt;

    switch (opcode) {
    case SOC_AVS_CTRL_PVTMON_VOLTAGE_GET:
        /* Should be called after soc_avs_pvtmon_voltage_get()
         * to make device/board specific adjustments.
         */
        value = *(uint32 *)data;
        *(uint32 *)data = (value * 880 * 1000) / (1024 * 70);
        break;
    
    case SOC_AVS_CTRL_PVTMON_TEMP_GET:
        value = *(uint32 *)data;
        *(int32 *)data = (41004000 - (48505 * value)) / 10000;
        break;
    
    case SOC_AVS_CTRL_QUERY_TRACK_STEP_SIZE:
        /* 5 mV */
        *(uint32 *)data = 50;
        break;
    
    case SOC_AVS_CTRL_PVTMON_VOLTAGE_SET:
        /* Should be called before soc_avs_pvtmon_voltage_set()
         * to calculate the dac value before updating pvtmon.
         */
        value = *(uint32 *)data;
        set_volt = (float)value / 10000;
        v_dac = (2 * (1 - set_volt)) +  0.549;

        /* Calculating Vbgu from OTP bits */
        SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLE_7r(unit, &rval));
        bg_val = soc_reg_field_get(unit, TOP_SWITCH_FEATURE_ENABLE_7r, rval, BOND_PVTMON_BGf);
        
        /* Calulating DAC value based on V_dac */
        dac_val_temp = (4.188 * (285 - v_dac / v_bg[bg_val] * 241 ));
        *(uint32 *)data = (uint32)dac_val_temp;
        break;
    
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
#endif
    
    return rv;
}

int
_soc_saber2_avs_info_init(int unit, soc_avs_info_t *avs_info)
{    
#ifdef SB2_AVS_I2C
    if (sb2_dac_devs_init(unit, I2C_MUX_3) < 0) {
        LOG_ERROR(BSL_LS_APPL_I2C,
                (BSL_META("ERROR: dac_devs_init fail\n")));
        return SOC_E_FAIL;
    }

#else
    SOC_IF_ERROR_RETURN(_soc_saber2_init_pvt(unit));
#endif

    if (avs_info == NULL) {
        return (SOC_E_PARAM);
    }
    
    avs_info->num_centrals = SOC_AVS_SABER2_NUMBER_OF_CENTRALS;
    avs_info->cent_xbmp[0] = SOC_AVS_SABER2_CENTRAL_XBMP0;
    avs_info->cent_xbmp[1] = SOC_AVS_SABER2_CENTRAL_XBMP1;
    avs_info->first_cent = SOC_AVS_SABER2_FIRST_CENTRAL;

    avs_info->num_remotes = SOC_AVS_SABER2_NUMBER_OF_REMOTES;
    avs_info->rmt_xbmp[0] = SOC_AVS_SABER2_REMOTE_XBMP0;
    avs_info->first_rmt = SOC_AVS_SABER2_FIRST_REMOTE;

    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_SABER2_VMIN_AVS);
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_SABER2_VMAX_AVS);
    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_SABER2_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_SABER2_VMARGIN_HIGH);
    
    avs_info->rosc_count_mode = SOC_AVS_ROSC_COUNT_MODE_1EDGE;
    avs_info->ref_clk_freq = 25;
    avs_info->measurement_time_control = 127;
    avs_info->osc_cen_init_info = _sb2_osc_cen_init_list;
    avs_info->osc_cen_thr_en_info = _sb2_osc_cen_thr_en_list;
    avs_info->osc_seq_reset_info = _sb2_osc_seq_reset_list;

    avs_info->avs_flags = SOC_AVS_INFO_F_SET_THRESHOLDS |
                          SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE; 

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        avs_info->avs_flags |= SOC_AVS_INFO_F_RSOC_COUNT_DMA;
        avs_info->cen_osc_reg = AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr;
        avs_info->rmt_osc_reg = AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr;
    }
#endif
    return SOC_E_NONE;
}

int
_soc_saber2_avs_info_deinit(int unit)
{
    return SOC_E_NONE;
}

soc_avs_functions_t _sb2_avs_functions;

int
soc_saber2_avs_init(int unit)
{
    /* register avs functions*/
    memset(&_sb2_avs_functions, 0, sizeof(soc_avs_functions_t));
    _sb2_avs_functions.info_init = &_soc_saber2_avs_info_init;
    _sb2_avs_functions.info_deinit = &_soc_saber2_avs_info_deinit;
    _sb2_avs_functions.ioctl = &_soc_saber2_avs_info_ioctl;
#if 0
    _sb2_avs_functions.voltage_set = &_soc_saber2_avs_voltage_set;
    _sb2_avs_functions.voltage_get = &_soc_saber2_avs_voltage_get;
#endif
    soc_avs_function_register(unit, &_sb2_avs_functions);
    return 0;
}
#endif /* INCLUDE_AVS */
