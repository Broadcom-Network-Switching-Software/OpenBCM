/*
 * rate_measurement.c
 *
 *  Created on: Dec 12, 2018
 *      Author: si888124
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <bcm/cosq.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dbal/dbal.h>

#include "rate_measurement_dbal.h"

/*
 * brief - verify function for dnx_rate_measurement_interval_set
 */
static shr_error_e
dnx_rate_measurement_interval_set_verify(
    int unit,
    int interval)
{
    uint32 max_interval = dnx_data_egr_queuing.rate_measurement.max_interval_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(interval, 0, max_interval, _SHR_E_PARAM, "interval %d is out of bounds", interval);
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_interval_set(
    int unit,
    int interval)
{
    dnxcmn_time_t time;
    int current_interval;
    uint32 hw_interval;
    int promile_update = FALSE;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_rate_measurement_interval_set_verify(unit, interval));

    /** get previous interval*/
    SHR_IF_ERR_EXIT(dnx_rate_measurement_interval_get(unit, &current_interval));

    if (interval != 0 && current_interval != interval)
    {
        promile_update = TRUE;
    }

    /** convert interval from usec to clocks */
    time.time_units = DNXCMN_TIME_UNIT_USEC;
    COMPILER_64_SET(time.time, 0, interval);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &hw_interval));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_interval_set(unit, hw_interval));

    if (promile_update)
    {
        /** go over all active ports and update the promile and granularity */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &logical_ports));
        BCM_PBMP_ITER(logical_ports, port)
        {
            SHR_IF_ERR_EXIT(dnx_rate_measurement_port_rate_update_set(unit, port));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_interval_get(
    int unit,
    int *interval)
{
    dnxcmn_time_t time;
    uint32 hw_interval;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_interval_get(unit, &hw_interval));

    /** convert interval from clocks to usec*/
    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get(unit, hw_interval, DNXCMN_TIME_UNIT_USEC, &time));

    *interval = COMPILER_64_LO(time.time);
exit:
    SHR_FUNC_EXIT;
}

/*
 * brief- verify function for dnx_rate_measurement_weight_set
 */
static shr_error_e
dnx_rate_measurement_weight_set_verify(
    int unit,
    int rate_weight)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (rate_weight)
    {
        case BCM_COSQ_RATE_WEIGHT_FULL:
        case BCM_COSQ_RATE_WEIGHT_HALF:
        case BCM_COSQ_RATE_WEIGHT_QUARTER:
        case BCM_COSQ_RATE_WEIGHT_EIGHTH:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Weight %d is invalid", rate_weight);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - convert BCM defines to DBAL rate weight enum
 */
static shr_error_e
dnx_rate_measurement_weight_to_dbal_enum_get(
    int unit,
    int rate_weight,
    dbal_enum_value_field_egress_rate_weight_e * dbal_rate_weight)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (rate_weight)
    {
        case BCM_COSQ_RATE_WEIGHT_FULL:
            *dbal_rate_weight = DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_FULL;
            break;
        case BCM_COSQ_RATE_WEIGHT_HALF:
            *dbal_rate_weight = DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_HALF;
            break;
        case BCM_COSQ_RATE_WEIGHT_QUARTER:
            *dbal_rate_weight = DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_QUARTER;
            break;
        case BCM_COSQ_RATE_WEIGHT_EIGHTH:
            *dbal_rate_weight = DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_EIGHTH;
            break;
        default:
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * brief - convert DBAL enum rate weight to BCM defines
 */
static shr_error_e
dnx_rate_measurement_dbal_enum_to_weight_get(
    int unit,
    dbal_enum_value_field_egress_rate_weight_e dbal_rate_weight,
    int *rate_weight)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_rate_weight)
    {
        case DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_FULL:
            *rate_weight = BCM_COSQ_RATE_WEIGHT_FULL;
            break;
        case DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_HALF:
            *rate_weight = BCM_COSQ_RATE_WEIGHT_HALF;
            break;
        case DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_QUARTER:
            *rate_weight = BCM_COSQ_RATE_WEIGHT_QUARTER;
            break;
        case DBAL_ENUM_FVAL_EGRESS_RATE_WEIGHT_EIGHTH:
            *rate_weight = BCM_COSQ_RATE_WEIGHT_EIGHTH;
            break;
        default:
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_weight_set(
    int unit,
    int rate_weight)
{
    dbal_enum_value_field_egress_rate_weight_e dbal_rate_weight = DBAL_NOF_ENUM_EGRESS_RATE_WEIGHT_VALUES;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_rate_measurement_weight_set_verify(unit, rate_weight));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_weight_to_dbal_enum_get(unit, rate_weight, &dbal_rate_weight));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_weight_set(unit, dbal_rate_weight));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_weight_get(
    int unit,
    int *rate_weight)
{
    dbal_enum_value_field_egress_rate_weight_e dbal_rate_weight = DBAL_NOF_ENUM_EGRESS_RATE_WEIGHT_VALUES;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_weight_get(unit, &dbal_rate_weight));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_enum_to_weight_get(unit, dbal_rate_weight, rate_weight));

exit:
    SHR_FUNC_EXIT;
}

/*
 * brief - calculate promile and granularity for the given egq if and configure to HW
 */
static shr_error_e
dnx_rate_measurement_config_set(
    int unit,
    bcm_core_t core,
    int egq_if,
    int rate)
{
    uint32 promile = 0, granularity = 0;
    uint64 promile_calc;
    uint32 interval;
    uint32 promile_factor = dnx_data_egr_queuing.rate_measurement.promile_factor_get(unit);
    uint32 core_clock = dnx_data_device.general.core_clock_khz_get(unit);
    uint32 min_bytes_for_granularity = dnx_data_egr_queuing.rate_measurement.min_bytes_for_granularity_get(unit);
    uint32 max_granularity = dnx_data_egr_queuing.rate_measurement.max_granularity_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    /** Get current interval in clocks */
    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_interval_get(unit, &interval));

    /*
     * calculate promile
     * promile[bytes] = (rate[Kbps] * interval[clocks]) / (promile_factor * core_clock[Kclocks/sec])
     */
    COMPILER_64_SET(promile_calc, 0, rate);
    COMPILER_64_UMUL_32(promile_calc, 10000);
    COMPILER_64_UMUL_32(promile_calc, interval);
    COMPILER_64_UDIV_32(promile_calc, promile_factor);
    COMPILER_64_UDIV_32(promile_calc, core_clock);
    promile = COMPILER_64_LO(promile_calc);

    /*
     * calculate granularity based on promile: promile * 2^granularity >= min_bytes_for_granularity 
     */
    granularity = utilex_log2_round_up(UTILEX_DIV_ROUND_UP(min_bytes_for_granularity * 10000, promile));
    if (granularity > max_granularity)
    {
        granularity = max_granularity;
    }
    promile = promile / 10000;
    /** write to HW */
    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_promile_set(unit, core, egq_if, promile, granularity));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_port_rate_update_set(
    int unit,
    int port)
{
    bcm_core_t core;
    int egq_if, rate;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_KBPS | DNX_ALGO_PORT_SPEED_F_TX, &rate));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_config_set(unit, core, egq_if, rate));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_compensation_set(
    int unit,
    bcm_core_t core_id,
    uint32 egq_if,
    int compensation)
{
    uint32 comp_abs, comp_sign;
    SHR_FUNC_INIT_VARS(unit);

    comp_abs = utilex_abs(compensation);
    comp_sign = (compensation < 0) ? 1 : 0;

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_compensation_set(unit, core_id, egq_if, comp_abs, comp_sign));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_compensation_get(
    int unit,
    bcm_core_t core_id,
    uint32 egq_if,
    int *compensation)
{
    uint32 comp_abs, comp_sign;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_compensation_get(unit, core_id, egq_if, &comp_abs, &comp_sign));

    *compensation = (comp_sign) ? (-1 * comp_abs) : comp_abs;
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_rate_measurement_counter_get(
    uint32 unit,
    bcm_core_t core_id,
    uint32 egq_if,
    uint32 *rate)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_counter_config_set(unit, core_id, egq_if));

    SHR_IF_ERR_EXIT(dnx_rate_measurement_dbal_rate_counter_get(unit, core_id, rate));

exit:
    SHR_FUNC_EXIT;
}
