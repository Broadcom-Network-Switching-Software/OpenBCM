/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pvtmon.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#ifdef  INCLUDE_AVS
#include <soc/avs.h>
#endif

#ifdef BCM_SABER2_SUPPORT
static int sb2_temperature_init[SOC_MAX_NUM_DEVICES] = {0};

static const soc_reg_t sb2_pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r
};

int
_soc_saber2_temperature_monitor_init(int unit) {
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    sal_usleep(1000);

    /* Bring PVTMON max and min data out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));


    sb2_temperature_init[unit] = 1;
    return SOC_E_NONE;
}

int
soc_sb2_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    uint32 rval;
    soc_reg_t reg;
    int index;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(sb2_pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(sb2_pvtmon_result_reg);
    }
    
    /* Check to see if the thermal monitor is initialized */
    if(sb2_temperature_init[unit] == 0) {
        SOC_IF_ERROR_RETURN(_soc_saber2_temperature_monitor_init(unit));
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = sb2_pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        cur = (41004000 - (48505 * fval)) / 10000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (41004000 - (48505 * fval)) / 10000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak = peak;
    }
    
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON1_MIN_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MAX_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                      THERMAL_PVTMON0_MIN_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));
    *temperature_count=num_entries_out;

    return SOC_E_NONE;
}

#ifdef  INCLUDE_AVS
int soc_sb2_volt_reading_reset(int unit)
{
    uint32 rval, rval1;
    uint32 m_init_pvt_mntr, m_init_pvt_mntr_orig;

    SOC_IF_ERROR_RETURN(READ_AVS_REG_HW_MNTR_SEQUENCER_INITr(unit, &rval1));
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SEQUENCER_INITr, &rval1,
            SEQUENCER_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_AVS_REG_HW_MNTR_SEQUENCER_INITr(unit, rval1));

    SOC_IF_ERROR_RETURN(
             READ_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, &rval));
    m_init_pvt_mntr = (soc_reg_field_get(unit,
                AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
                rval, M_INIT_PVT_MNTRf));
    m_init_pvt_mntr_orig = m_init_pvt_mntr;

    /* Set bits corresponding to required voltage */
    m_init_pvt_mntr |= 0x78; /* 1V(0), 1V(1), 1.8V, 3.3V*/
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
            &rval, M_INIT_PVT_MNTRf, m_init_pvt_mntr);
    SOC_IF_ERROR_RETURN(
            WRITE_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, rval));

    /* Clear the fields */
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
            &rval, M_INIT_PVT_MNTRf, m_init_pvt_mntr_orig);
    SOC_IF_ERROR_RETURN(
            WRITE_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, rval));

    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SEQUENCER_INITr, &rval1,
            SEQUENCER_INITf, 0);
    SOC_IF_ERROR_RETURN(WRITE_AVS_REG_HW_MNTR_SEQUENCER_INITr(unit, rval1));

    return SOC_E_NONE;
}

int soc_sb2_volt_get(int unit, int pvtmon, int *volt_reading)
{
    uint32 rval;
    int valid = 0, done = 0, volt_data, volt_data_sum;
    int i, num_readings = 10;
    soc_reg_t reg;
    soc_timeout_t to;
    int timeout_val = 10000; /* in usecs */
    int div;
    soc_timeout_init(&to, timeout_val, 0);

    switch(pvtmon) {
        case SOC_AVS_PVT_1V_0:
            reg = AVS_REG_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUSr;
            div = 7;
            break;
        case SOC_AVS_PVT_1V_1:
            reg = AVS_REG_RO_REGISTERS_0_PVT_1V_1_MNTR_STATUSr;
            div = 7;
            break;
        case SOC_AVS_PVT_1P8V:
            reg = AVS_REG_RO_REGISTERS_0_PVT_1P8V_MNTR_STATUSr;
            div = 4;
            break;
        case SOC_AVS_PVT_3P3V:
            reg = AVS_REG_RO_REGISTERS_0_PVT_3P3V_MNTR_STATUSr;
            div = 2;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,"Invalid PVTMON\n")));
            return SOC_E_FAIL;
    }
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        valid = soc_reg_field_get(unit, reg, rval, VALID_DATAf);
        done =  soc_reg_field_get(unit, reg, rval, DONEf);
        if((valid == 1) && (done == 1)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "PVTMON %d monitor timeout\n"), pvtmon));
            break;
        }
    } while(TRUE);

    if((valid == 1) && (done == 1)) {
        volt_data_sum = 0;
        for(i = 0; i < num_readings; i++) {
            volt_data = soc_reg_field_get(unit, reg, rval, DATAf);
            volt_data_sum += volt_data;
            sal_udelay(10);
        }
        volt_data = volt_data_sum / num_readings;
        *volt_reading = ((volt_data * 880 * 10) / (1024 * div));
    }
    return SOC_E_NONE;
}


int
soc_sb2_show_voltage(int unit)
{
    int pvtmon;
    int avg;
    int pvtmon_list[] = {SOC_AVS_PVT_1V_0, SOC_AVS_PVT_1V_1,
                       SOC_AVS_PVT_1P8V, SOC_AVS_PVT_3P3V};
    char *pvtmon_str[] = {"ROV_PVTMON_CORE_1P0 1 - 1.0V(0)",
                          "ROV_PVTMON_1P0 1      - 1.0V(1)",
                          "ROV_PVTMON_1P8 1      - 1.8V   ",
                          "ROV_PVTMON_3P3 1      - 3.3V   "};

    SOC_IF_ERROR_RETURN(soc_sb2_volt_reading_reset(unit));
    /* Read Voltages */
    for (pvtmon = 0; pvtmon < COUNTOF(pvtmon_list); pvtmon++) {
        SOC_IF_ERROR_RETURN(soc_sb2_volt_get(unit, pvtmon_list[pvtmon], &avg));
        LOG_CLI((BSL_META_U(unit,
                            "%s : Average  = %d.%03dV\n"), pvtmon_str[pvtmon],
                 (avg/1000), (avg %1000)));
    }

    return SOC_E_NONE;
}
#else
int
soc_sb2_show_voltage(int unit)
{
    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "AVS needs to be included to display voltage\n")));
    return SOC_E_UNAVAIL;
}
#endif

#endif
