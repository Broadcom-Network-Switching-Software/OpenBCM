/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.c
 * Purpose: AVS (Adaptive Voltage Scaling) commands.
 * AVS is a power-saving technique of the digital 1.0V supply while maintaining
 * performance under various process and operating conditions.
 * Commands in this file are provided for AVS algorithm. Also some helper commands
 * are also provided for the AVS algorithm verificatiion/validation.
 * AVS algorithm commands:
 *  avs init : the first command should be invoked to setup the software data
 *      and chip specific information for AVS algorithm.
 *  avs openloop : read the device otp to get the working voltage and set to the
 *      vrm.
 *  avs start :(close-loop) perform the predict algorithm to get the converged
 *      voltage w/ margin and then set the value to external voltage supply.
 *  avs track :(close-loop) perform track algorithm to monitor the ROSC count
 *      then adjust the voltage accordingly. Interval value here is used to
 *      determine to do the track periodically or just one time.
 * AVS helper commands:
 *  avs vrm : Add default vrm profile.
 *  avs xbmp : Not all the ROSC counts are required for the algorithm
 *      calculation to derive the converged voltage. xbmp specify the ROSCs
 *      which will be excluded from the algorithm. This command is usually used
 *      during the algorithm validation for new chip support.
 *  avs CoreVoltage : Set or get the CoreVoltage which is the main voltage
 *      supply for the chip.
 *  avs rov: Get the recommended operating voltage (ROV).
 *  avs margin : Specify the margin value used for the converged method.
 *      This command is usually used during the algorithm validation
 *      for new chip support.
 *  avs vpred : get the predicted voltage according to the specific v1 and v2.
 *      This command is usually used during the algorithm validation.
 *  avs Count : get the ROSC count.
 *      This command is usually used during the algorithm validation.
 *  avs SHow : dump the chip specific avs information.
 *  avs PVTmon : get core voltage (0.1 mV), temperature (mC) using pvtmon.
 *  avs deinit : release avs related software resources.
 * Requires:
 */


#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>

#ifdef  INCLUDE_AVS
#include <soc/avs.h>

char cmd_avs_usage[] =
    "avs command usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    " avs <option> [args...] \n"
#else
    "avs init \n\t"
    "   - initialize the avs software resource \n\t"
    "avs vrm dev=<dev_name> [mux=<mux_dev_name> mux_value=<enable value>]\n\t"
    "   - specify the default vrm profile.\n\t"
    "   Note : Only support the pmbus compatiable device as default vrm.\n\t"
    "avs openloop \n\t"
    "   - Set the otp specified voltage to the system."
    "avs start \n\t"
    "   - start the avs close loop processing \n\t"
    "avs track [Interval=<usec>] \n\t"
    "   - start to adjust voltage with interval value. default is 1 sec \n\t"
    "     if Interval = 0, avs track will be stopped. \n\t"
    "     if Interval = -1, avs track will be invoked once. \n\t"
    "avs xbmp Osc=Central|Remote [StartOsc=<osc> [NumOsc=<num>]]"
    " [Value=<val>]\n\t"
    "   - set/get the Central or Remote Oscillator exclude bitmap \n\t"
    "   for example: \n\t"
    "    avs xbmp o=c ==> show cen_osc_xbmp\n\t"
    "    avs xbmp o=c so=5 no=3 v=1 ==> cen_osc_xbmp[5,6,7]=0x1\n\t"
    "avs CoreVoltage [Voltage=<voltage>] [StepSize=<step_size>]]\n\t"
    "   - set/get the core voltage in 0.1mV units \n\t"
    "   if step_size is specified, the core voltage will be inc/dec by step_size\n\t"
    "   if step_size = -1, the core voltage will be set directly.\n\t"
    "   if step_size is omitted, the default step size (10mV) will be taken\n\t"
    "   continuously up to the target voltage.\n\t"
    "avs rov \n\t"
    "   - Get the recommended operating voltage (ROV)"
    "avs Margin [High=<hval> Low=<lval>] \n\t"
    "   - set/get margin high and margin low \n\t"
    "avs RESetOSCillators\n\t"
    "   - Reset all the oscillators.\n\t"
    "avs Vpred <v1s> <v2s>  \n\t"
    "   - compute Vpred(v3) for given v1s, v2s \n\t"
    "avs Count Osc=Central|Remote\n\t"
    "   - Get the Central or Remote Oscillator count \n\t"
    "avs SHow \n\t"
    "avs PVTmon Type=Voltage|Temperature\n\t"
    "   - Read the core voltage (0.1 mV), temperature (mC) using pvtmon\n\t"
    "avs deinit \n\t"
    "   - release avs related software resource \n"
#endif
;
#define AVS_ERROR_RETURN(op)                        \
  do {                                              \
        int _rv;                                    \
        if (SOC_FAILURE((_rv = (op)))) {            \
            cli_out("Error: %s\n", soc_errmsg(_rv));\
            return CMD_FAIL;                        \
        }                                           \
  } while (0)

/* 0.84 V */
#define AVS_ABS_MIN_CVOLT SOC_AVS_UINT(8400)
/* 1.10 V */
#define AVS_ABS_MAX_CVOLT SOC_AVS_UINT(11000)

/* 0.01 V = 10 mV */
#define AVS_DEFAULT_CVOLT_INC_STEP_SIZE SOC_AVS_UINT(100)
#define AVS_DEFAULT_CVOLT_DEC_STEP_SIZE SOC_AVS_UINT(100)

STATIC int
_avs_core_voltage_step (int unit, uint32 req_cvolt, uint32 svolt)
{
    uint32 cur_cvolt;
    uint32 inc_svolt, dec_svolt;

    if (svolt == 0) {
        inc_svolt = AVS_DEFAULT_CVOLT_INC_STEP_SIZE;
        dec_svolt = AVS_DEFAULT_CVOLT_DEC_STEP_SIZE;
    } else {
        inc_svolt = svolt;
        dec_svolt = svolt;
    }
    SOC_IF_ERROR_RETURN(soc_avs_voltage_get(unit, &cur_cvolt));
    if (req_cvolt >= cur_cvolt) {
        while (req_cvolt >= (cur_cvolt + inc_svolt)) {
            SOC_IF_ERROR_RETURN(
                soc_avs_voltage_set(unit, cur_cvolt + inc_svolt));
            SOC_IF_ERROR_RETURN(
                soc_avs_voltage_get(unit, &cur_cvolt));
        }
    } else { /* req_cvolt < cur_cvolt */
        while (cur_cvolt >= (req_cvolt + dec_svolt)) {
            SOC_IF_ERROR_RETURN(
                soc_avs_voltage_set(unit, cur_cvolt - dec_svolt));
            SOC_IF_ERROR_RETURN(
                soc_avs_voltage_get(unit, &cur_cvolt));
        }
    }
    if (req_cvolt != cur_cvolt) {
        SOC_IF_ERROR_RETURN(
            soc_avs_voltage_set(unit, req_cvolt));
    }

    return (SOC_E_NONE);
}
const char *diag_parse_osc_type[] = {
    "Central",
    "Remote",
    NULL
};
const char *diag_parse_pvt_type[] = {
 "Voltage",
 "Temperature",
 NULL
};

cmd_result_t
cmd_avs(int unit, args_t *a)
{
    char *subcmd;
    parse_table_t pt;
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "init") == 0) {
        AVS_ERROR_RETURN(
            soc_avs_init(unit));
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "deinit") == 0) {
        AVS_ERROR_RETURN(
            soc_avs_deinit(unit));
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "start") == 0) {
        AVS_ERROR_RETURN(
            soc_avs_start(unit));
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "track") == 0) {
        int usec = 1000000;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Interval", PQ_DFL | PQ_INT,
                        (void *)( 0), &usec, NULL);
        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return(CMD_FAIL);
        }
        parse_arg_eq_done(&pt);
        if (usec == -1) {
            AVS_ERROR_RETURN(
                soc_avs_track(unit));
            cli_out("soc_avs_track once\n");
        } else if (usec < 0) {
            cli_out("Error: Invalid interval: %d\n", usec);
            return CMD_USAGE;
        } else if (usec == 0) {
            AVS_ERROR_RETURN(
                soc_avs_track_stop(unit));
            cli_out("soc_avs_track stopped !\n");
        } else {
            AVS_ERROR_RETURN(
                soc_avs_track_start(unit, usec));
            cli_out("soc_avs_track start with interval=%d usec\n",usec);
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "xbmp") == 0) {
        int osc_type, start_osc, num_osc, value;
        if ((ARG_CNT(a)) == 0) {
            AVS_ERROR_RETURN(
                soc_avs_xbmp_dump(unit, SOC_AVS_ROSC_TYPE_CENTRAL));
            AVS_ERROR_RETURN(
                soc_avs_xbmp_dump(unit, SOC_AVS_ROSC_TYPE_REMOTE));
            return CMD_OK;
        }
        osc_type = -1;
        start_osc = -1;
        num_osc = 1;
        value = 1;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Osc", PQ_MULTI | PQ_DFL, 0, &osc_type,
                        diag_parse_osc_type);
        parse_table_add(&pt, "StartOsc", PQ_INT | PQ_DFL, 0, &start_osc, NULL);
        parse_table_add(&pt, "NumOsc", PQ_INT | PQ_DFL, 0, &num_osc, NULL);
        parse_table_add(&pt, "Value", PQ_INT | PQ_DFL, 0, &value, NULL);

        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (osc_type == -1) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        if (start_osc == -1) {
            AVS_ERROR_RETURN(
                soc_avs_xbmp_dump(unit, osc_type));
            return CMD_OK;
        }
        AVS_ERROR_RETURN(
            soc_avs_xbmp_set(unit, osc_type, start_osc, num_osc, value));
        return CMD_OK;
    }
    if (parse_cmp("CoreVoltage", subcmd, 0)) {
        uint32 voltage = 0;
        int step = 0;
        if ((ARG_CNT(a)) == 0) {
            AVS_ERROR_RETURN(
                soc_avs_voltage_get(unit, &voltage));
            cli_out("avs corevoltage get (in 0.1mV): %d\n",
                voltage);
            return CMD_OK;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Voltage", PQ_INT | PQ_DFL, 0, &voltage, NULL);
        parse_table_add(&pt, "StepSize", PQ_INT | PQ_DFL, 0, &step, NULL);

        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        if (voltage == 0) {
            cli_out("Error: voltage is not specified %s\n", ARG_CUR(a));
            return CMD_USAGE;
        }
        if (step == -1) {
            AVS_ERROR_RETURN(
                soc_avs_voltage_set(unit, voltage));
            cli_out("avs corevoltage set (in 0.1mV): %d\n",
                    voltage);
        } else {
            AVS_ERROR_RETURN(
                _avs_core_voltage_step(unit, voltage, step));
            cli_out("avs corevoltage set (in 0.1mV): %d with step %d\n",
                    voltage, (step) ? step : AVS_DEFAULT_CVOLT_INC_STEP_SIZE);
        }
        return CMD_OK;
    }
    if (parse_cmp("Margin", subcmd, 0)) {
        soc_avs_debug_margin_t avs_debug_margin;
        if ((ARG_CNT(a)) == 0) {
            AVS_ERROR_RETURN(
                soc_avs_debug_margin_get(unit, &avs_debug_margin));
            cli_out("avs custom margin: high=%d low=%d\n",
                    avs_debug_margin.vmargin_high,
                    avs_debug_margin.vmargin_low);
            return CMD_OK;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "High", PQ_INT, 0,
                        &avs_debug_margin.vmargin_high, NULL);
        parse_table_add(&pt, "Low", PQ_INT, 0,
                        &avs_debug_margin.vmargin_low, NULL);

        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        AVS_ERROR_RETURN(
            soc_avs_debug_margin_set(unit, &avs_debug_margin));
        return CMD_OK;
    }
    if (parse_cmp("Vpred", subcmd, 0)) {
        uint32 v1, v2, vpred, vlow, vhigh;
        char *cmd_arg;
        if ((cmd_arg = ARG_GET(a)) == NULL){
            return CMD_USAGE;
        } else {
            v1 = parse_integer(cmd_arg);
        }
        if ((cmd_arg = ARG_GET(a)) == NULL){
            return CMD_USAGE;
        } else {
            v2 = parse_integer(cmd_arg);
        }
        AVS_ERROR_RETURN(
            _soc_avs_predict_vpred(unit, -1, v1, v2, &vpred, &vlow, &vhigh));
        cli_out("avs predict(in 0.1mV): v1=%d v2=%d vpred=%d\n",
                v1, v2, vpred);
        return CMD_OK;
    }
    if (parse_cmp("Count", subcmd, 0)) {
        int osc_type = -1;
        if ((ARG_CNT(a)) == 0) {
            AVS_ERROR_RETURN(
                soc_avs_osc_count_dump(unit, SOC_AVS_ROSC_TYPE_ALL));
            return CMD_OK;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Osc", PQ_MULTI | PQ_DFL, 0, &osc_type,
                        diag_parse_osc_type);

        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (osc_type == -1) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        AVS_ERROR_RETURN(
            soc_avs_osc_count_dump(unit, osc_type));
        return CMD_OK;
    }
    if (parse_cmp("SHow", subcmd, 0)) {
        AVS_ERROR_RETURN(
            soc_avs_info_dump(unit));
        return CMD_OK;
    }
    if (parse_cmp("PVTmon", subcmd, 0)) {
        int pvt_type = -1;
        int32 temperature = 0;
        uint32 voltage = 0;
        if ((ARG_CNT(a)) == 0) {
            AVS_ERROR_RETURN(
                soc_avs_pvtmon_voltage_get(unit, &voltage));
                cli_out("avs pvtmon voltage get (in 0.1 mV): %d\n", voltage);
            AVS_ERROR_RETURN(
                soc_avs_temperature_get(unit, &temperature));
                cli_out("avs temperature get (in mC): %d\n", temperature);
            return CMD_OK;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Type", PQ_MULTI | PQ_DFL, 0, &pvt_type,
                        diag_parse_pvt_type);
        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (pvt_type == -1) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        if (pvt_type == 0) {
            AVS_ERROR_RETURN(
                soc_avs_pvtmon_voltage_get(unit, &voltage));
            cli_out("avs pvtmon voltage get (in 0.1 mV): %d\n", voltage);
            return CMD_OK;
        }
        if (pvt_type == 1) {
            AVS_ERROR_RETURN(
                soc_avs_temperature_get(unit, &temperature));
                cli_out("avs temperature get (in mC): %d\n", temperature);
            return CMD_OK;
        }
        return CMD_USAGE;
    }
    if (parse_cmp("RESetOSCillators", subcmd, 0)) {
        AVS_ERROR_RETURN(
            _soc_avs_initialize_oscs(unit));
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "openloop") == 0) {
            AVS_ERROR_RETURN(soc_avs_openloop_main(unit));
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "rov") == 0) {
        uint32 voltage = 0;
        AVS_ERROR_RETURN(soc_avs_openloop_voltage_get(unit, &voltage));
        cli_out("Recommended operating voltage(ROV): %dmV\n", voltage);
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "vrm") == 0) {
        soc_avs_vrm_profile_t vrm_profile;
        char *mux_name = NULL, *dev_name = NULL;
        int8 mux_value;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "mux", PQ_DFL|PQ_STRING, NULL,
                        &mux_name, NULL);
        parse_table_add(&pt, "mux_value", PQ_INT8, 0,
                        &mux_value, NULL);
        parse_table_add(&pt, "dev", PQ_DFL|PQ_STRING, NULL,
                        &dev_name, NULL);
        if (parse_arg_eq(a, &pt) <= 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        sal_memset(&vrm_profile, 0, sizeof(soc_avs_vrm_profile_t));
        if (mux_name) {
            sal_strncpy(vrm_profile.mux_name, mux_name,
                        sizeof(vrm_profile.mux_name) - 1);
        }
        if (dev_name) {
            sal_strncpy(vrm_profile.vrm_name, dev_name,
                        sizeof(vrm_profile.vrm_name) - 1);
        }
        vrm_profile.mux_enable_value = (uint8)mux_value;
        parse_arg_eq_done(&pt);
        AVS_ERROR_RETURN(soc_avs_vrm_profile_add(unit, &vrm_profile));
        return CMD_OK;
    }
    return CMD_USAGE;
}

#else  /* INCLUDE_AVS */

typedef int _diag_avs_c_not_empty; /* Make ISO compilers happy. */

#endif /* INCLUDE_AVS */
