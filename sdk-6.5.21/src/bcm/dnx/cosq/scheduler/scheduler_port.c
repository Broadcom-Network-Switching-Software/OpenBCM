
/** \file scheduler_port.c
 * 
 *
 * e2e scheduler functionality for DNX
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
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

#include "scheduler_dbal.h"
#include "scheduler_element.h"
#include "scheduler_port.h"
#include "scheduler_allocators.h"

#define DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_if) ((sch_if) < dnx_data_sch.interface.nof_channelized_calendars_get(unit))

/**
 * \brief - Get number of calendar ticks in one second
 */
static shr_error_e
dnx_scheduler_port_calendar_ticks_per_sec_get(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 *ticks_per_sec)
{
    uint32 access_period, cal_length, device_ticks_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_calendar_params_get(unit, DBAL_CORE_DEFAULT, port_calendar_type,
                                                            &access_period, &cal_length));

    device_ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);
    *ticks_per_sec = UTILEX_DIV_ROUND_UP(device_ticks_per_sec, (cal_length * access_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * Convert a rate given in Kbits/sec units to quanta
 */
static shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint64 rate,                /* in Kbits/sec */
    uint32 credit_div,          /* REBOUNDED/ASSIGNED_CREDIT */
    uint32 *quanta              /* in device clocks */
    )
{
    uint32 calc2, calc;
    uint32 credit_worth;
    uint64 temp_calc;
    uint64 zero = 0;

    uint32 ticks_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    if (COMPILER_64_EQ(zero, rate))
    {
        *quanta = 0;
    }
    else
    {
        /** get credit worth*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        /** get ticks per second */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        /** calc2 = rate / calc [1/clocks] */
        SHR_IF_ERR_EXIT(dnx_algo_cal_u64_div_long_and_round_up(unit, rate, calc, &temp_calc));
        if (COMPILER_64_HI(temp_calc) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "overflow from 32-bits\n");
        }

        calc2 = (calc == 0 ? 0 : COMPILER_64_LO(temp_calc));

        *quanta = (calc2 == 0 ? 1 : calc2);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * Convert a rate given in Kbits/sec units to quanta
 */
shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 rate,                /* in Kbits/sec */
    uint32 *quanta              /* in device clocks */
    )
{
    uint32 divider;
    uint64 temp_rate;

    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_SET(temp_rate, 0, rate);

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &divider));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider
                    (unit, port_calendar_type, temp_rate, divider, quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Convert quanta given in
 *   device clocks to rate in Kbits/sec units.
 */
shr_error_e
dnx_scheduler_port_quanta_to_rate_kbits_per_sec(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 quanta,              /* in device clocks */
    uint32 *rate                /* in Kbits/sec */
    )
{
    UTILEX_U64 calc2;
    uint32 calc, tmp;
    uint32 credit_worth;
    uint32 ticks_per_sec;
    uint32 credit_div;          /* REBOUNDED/ASSIGNED_CREDIT */

    SHR_FUNC_INIT_VARS(unit);

    if (0 == quanta)
    {
        *rate = 0;
    }
    else
    {
        /** get credit worth*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        utilex_u64_multiply_longs(calc, quanta, &calc2);
        if (utilex_u64_to_long(&calc2, &tmp))
        {
            /** Overflow */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Overflow when calculating rate\n");
        }

        *rate = tmp;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate maximal possible divider (assigned credit worth)
 * which can provide desired max rate
 */
static shr_error_e
dnx_scheduler_port_shaper_calculate_divider(
    int unit,
    uint32 *divider)
{
    uint32 new_quanta, curr_divider, iterate_divider;

    uint64 max_rate;
    uint32 max_divider;
    uint32 max_quanta_to_add;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(max_rate, 0, dnx_data_sch.ps.max_port_rate_mbps_get(unit));
    COMPILER_64_UMUL_32(max_rate, 1000);

    /** Get maximal value of credit worth field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_SCH_PORT_CREDIT_CONFIGURATION,
                                                          DBAL_FIELD_CREDIT_WORTH, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                          &max_divider));

    /** Get maximal value of quanta to add field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER,
                                                          DBAL_FIELD_QUANTA_TO_ADD, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                          &max_quanta_to_add));

    curr_divider = 1;

    for (iterate_divider = 1; iterate_divider < max_divider; iterate_divider++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(unit, DNX_SCH_PORT_TC_CALENDAR,
                                                                                    max_rate, iterate_divider,
                                                                                    &new_quanta));
        if (new_quanta > max_quanta_to_add)
        {
            break;
        }
        curr_divider = iterate_divider;
    }

    *divider = curr_divider;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert port TC/TCG max burst bytes to internal representaion
 */
static shr_error_e
dnx_scheduler_port_shaper_max_burst_to_internal(
    int unit,
    uint32 max_burst,
    uint32 *internal)
{
    uint32 credit_worth, credit_div;

    SHR_FUNC_INIT_VARS(unit);

    /** get credit worth*/
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

    *internal = max_burst * credit_div / credit_worth;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert port TC/TCG max burst internal representaion to bytes
 */
shr_error_e
dnx_scheduler_port_shaper_max_burst_from_internal(
    int unit,
    uint32 internal,
    uint32 *max_burst)
{
    uint32 credit_worth, credit_div;

    SHR_FUNC_INIT_VARS(unit);

    /** get credit worth*/
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

    *max_burst = internal * credit_worth / credit_div;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init assigned credit worth
 */
static shr_error_e
dnx_scheduler_port_credit_divider_init(
    int unit)
{
    uint32 new_divider = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_calculate_divider(unit, &new_divider));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_set(unit, new_divider));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init Scheduler port TC/TCG calendar configurations
 */
shr_error_e
dnx_scheduler_port_shaper_calendar_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** init CSC/PCS calendar configuration */
    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_config_init(unit));

    /** init assigned credit worth */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_credit_divider_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set maximal credit rate of Port TC
 */
shr_error_e
dnx_scheduler_port_priority_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 rate)
{
    uint32 quanta;
    int core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** calculate quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TC_CALENDAR, rate, &quanta));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_bandwidth_set(unit, core, base_hr + priority_ndx, quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get maximal credit rate of port TC
 */
shr_error_e
dnx_scheduler_port_priority_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int priority_ndx,
    uint32 *rate)
{

    uint32 quanta;
    int core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_bandwidth_get(unit, core, base_hr + priority_ndx, &quanta));

    /** calculate rate from quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TC_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set maximal credit rate of TCG
 */
shr_error_e
dnx_scheduler_port_tcg_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 rate)
{

    uint32 quanta;
    int core;
    int base_hr, global_tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** calculate quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TCG_CALENDAR, rate, &quanta));

    global_tcg = base_hr + tcg_ndx;

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_set(unit, core, global_tcg, quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get maximal credit rate of TCG
 */
shr_error_e
dnx_scheduler_port_tcg_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 *rate)
{

    uint32 quanta;
    int core;
    int base_hr, global_tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    global_tcg = base_hr + tcg_ndx;

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_get(unit, core, global_tcg, &quanta));

    /** calculate rate from quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TCG_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set max burst of port TC
 */
shr_error_e
dnx_scheduler_port_priority_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 max_burst)
{
    int core, base_hr;
    uint32 internal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** convert max burst to internal representation */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_to_internal(unit, max_burst, &internal));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_set(unit, core, base_hr + port_priority, internal));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get max burst of port TC
 */
shr_error_e
dnx_scheduler_port_priority_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 *max_burst)
{
    int core, base_hr;
    uint32 internal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_get(unit, core, base_hr + port_priority, &internal));

    /** convert max burst internal representation to bytes */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, internal, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  reset max burst of port TC
 */
shr_error_e
dnx_scheduler_port_priority_max_burst_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_reset(unit, core, base_hr + port_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set max burst of port TCG
 */
shr_error_e
dnx_scheduler_port_tcg_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 max_burst)
{
    int core, base_hr;
    uint32 internal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** convert max burst to internal representation */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_to_internal(unit, max_burst, &internal));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_set(unit, core, base_hr + tcg_index, internal));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get max burst of port TCG
 */
shr_error_e
dnx_scheduler_port_tcg_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 *max_burst)
{
    int core, base_hr;
    uint32 internal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_get(unit, core, base_hr + tcg_index, &internal));

    /** convert max burst internal representation to bytes */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, internal, max_burst));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  reset max burst of port TCG to default value
 */
shr_error_e
dnx_scheduler_port_tcg_max_burst_reset(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_reset(unit, core, base_hr + tcg_index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e
dnx_scheduler_hr_port_enable_set(
    int unit,
    int core,
    int base_hr,
    int nof_priorities,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!enable)
    {
        SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, TRUE));
    }

    SHR_IF_ERR_EXIT(dnx_sch_port_hr_enable_range_set(unit, core, base_hr, nof_priorities, enable));

    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e
dnx_scheduler_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    SHR_IF_ERR_EXIT(dnx_scheduler_hr_port_enable_set(unit, core, base_hr, nof_priorities, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get number of SCH interfaces for the provided port
 */
static shr_error_e
dnx_scheduler_port_nof_interfaces_get(
    int unit,
    bcm_port_t port,
    int *nof_interfaces)
{
    int priority_propagation_enable, sch_priorities_nof;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &sch_priorities_nof));
    *nof_interfaces = (priority_propagation_enable ? sch_priorities_nof : 1);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  get group (color) which currently has the lowest total bandwidth
 */
static int
dnx_scheduler_port_hr_min_bw_group_get(
    int unit,
    int core,
    int *group)
{

    int i, res;
    int min_bw = 0, group_bw = 0;

    SHR_FUNC_INIT_VARS(unit);

    *group = 0;
    res = cosq_config.groups_bw.get(unit, core, *group, &group_bw);
    SHR_IF_ERR_EXIT(res);
    min_bw = group_bw;

    for (i = 0; i < dnx_data_sch.se.nof_color_group_get(unit); i++)
    {
        res = cosq_config.groups_bw.get(unit, core, i, &group_bw);
        SHR_IF_ERR_EXIT(res);
        if (group_bw < min_bw)
        {
            min_bw = group_bw;
            *group = i;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  calculate group (color) with the lowest total bandwidth to 
 *  color the provide port
 * \note - this function updates swstate which is used for color calculation
 */
static int
dnx_scheduler_port_hr_group_calc(
    int unit,
    bcm_port_t port,
    int *group)
{
    uint32 res, se_id;
    int group_rate, hr_rate;
    int base_hr, interface_max_rate = 0;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &interface_max_rate));

    /** get priority 0 HR se_id (all port HRs are associated to the same group) */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr);

    res = cosq_config.hr_group_bw.get(unit, core, base_hr, &hr_rate);
    SHR_IF_ERR_EXIT(res);

    /** if group was already set, try to switch the group */
    if (hr_rate)
    {
        SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core, se_id, group));

        SHR_IF_ERR_EXIT(cosq_config.groups_bw.get(unit, core, *group, &group_rate));

        if ((group_rate - hr_rate) > 0)
        {
            group_rate = group_rate - hr_rate;
        }
        else
        {
            group_rate = 0;
        }
        SHR_IF_ERR_EXIT(cosq_config.groups_bw.set(unit, core, *group, group_rate));
    }

    /** update new group and hr rate */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_min_bw_group_get(unit, core, group));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.get(unit, core, *group, &group_rate));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.set(unit, core, *group, (group_rate + interface_max_rate)));
    SHR_IF_ERR_EXIT(cosq_config.hr_group_bw.set(unit, core, base_hr, interface_max_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Color all HRs of the provided port with given color group
 */
int
dnx_scheduler_port_hr_color_group_set(
    int unit,
    bcm_port_t logical_port,
    int priority,
    int group)
{
    int se_id, base_hr;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /**  Assign scheduler group */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** get se id from base HR and priority */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    /** set new group for se id */
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core, se_id, group));

    /** mark that the HR is colored */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get HR color group
 */
int
dnx_scheduler_port_hr_color_group_get(
    int unit,
    bcm_gport_t logical_port,
    int priority,
    int *group)
{
    int se_id;
    bcm_core_t core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    /**  Assign scheduler group */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** get se id from base HR and priority */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    /** get group from se id*/
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core, se_id, group));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Color all HRs of the provided port 
 * with the color which currently has lowest bandwidth.
 * max interface speed is considered as port bandwidth
 */
int
dnx_scheduler_port_hr_color_decide_and_set(
    int unit,
    bcm_port_t logical_port)
{
    int group;
    int priority_i;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** all the HRs in a port will have the same group (limitation cause of dual shapers) */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_group_calc(unit, logical_port, &group));      /* get the new group */

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
        /** color all HRs priorities*/
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_group_set(unit, logical_port, priority_i, group));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Uncolor all HRs of the provided port 
 */
int
dnx_scheduler_port_hr_color_unset(
    int unit,
    bcm_port_t logical_port)
{
    int priority, nof_priorities;
    int base_hr;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    for (priority = 0; priority < nof_priorities; priority++)
    {
        /** mark that the HR is not colored */
        SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, FALSE));

        
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure number of port priorities for the provided port
 */
int
dnx_scheduler_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core_id;
    int nof_priorities;
    dbal_enum_value_field_nof_ps_priorities_e ps_priority = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));

    switch (nof_priorities)
    {
        case 1:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY;
            break;
        case 2:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES;
            break;
        case 4:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES;
            break;
        case 8:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected number of priorities %d\n", nof_priorities);
            break;
    }

    SHR_IF_ERR_EXIT(dnx_sch_port_priorities_nof_set(unit, core_id, base_hr, ps_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * clear number of port priorities for the provided port
 */
int
dnx_scheduler_port_priorities_nof_clear(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core_id;
    uint32 allocated_hrs[1];
    int ps_priority;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core_id, base_hr / dnx_data_sch.ps.nof_hrs_in_ps_get(unit),
                                              allocated_hrs, &ps_priority, &priority_propagation_enable));
    if (allocated_hrs[0] == 0)
    {
        /** no HR is allocated on the PS */
        SHR_IF_ERR_EXIT(dnx_sch_port_priorities_nof_clear(unit, core_id, base_hr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure number of priority propagation priorities for the provided port
 */
int
dnx_scheduler_port_priority_propagation_priorities_nof_set(
    int unit,
    bcm_port_t logical_port)
{
    int e2e_if;
    int core_id;
    int nof_priorities;
    dbal_enum_value_field_nof_ps_priorities_e if_priority = 0;
    int priority_propagation_enable;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &e2e_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_dbal_port_priority_propagation_enable_set(unit, core_id, base_hr,
                                                                      priority_propagation_enable));

    switch (nof_priorities)
    {
        case 1:
            if_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY;
            break;
        case 2:
            if_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES;
            break;
        case 4:
            if_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES;
            break;
        case 8:
            if_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected number of priorities %d\n", nof_priorities);
            break;
    }

    SHR_IF_ERR_EXIT(dnx_sch_dbal_priority_propagation_priorities_nof_set(unit, core_id, e2e_if, if_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * clear number of priority propagation priorities for the provided port
 */
int
dnx_scheduler_port_priority_propagation_priorities_nof_clear(
    int unit,
    bcm_port_t logical_port)
{
    int e2e_if;
    int core_id;
    int should_clear;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &e2e_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_is_tag_clear(unit, core_id, e2e_if, &should_clear));

    if (should_clear)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_priority_propagation_priorities_nof_clear(unit, core_id, e2e_if));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * map all port HRs to TCG 0 as the default
 */
int
dnx_scheduler_port_tc_to_tcg_map_init(
    int unit,
    bcm_port_t logical_port)
{
    int nof_priorities;
    int tc;
    int priority_propagation_enable;
    dnx_scheduler_port_priority_propagation_version_t priority_propagation_version;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    priority_propagation_version = dnx_data_sch.general.port_priority_propagation_version_get(unit);

    for (tc = 0; tc < nof_priorities; tc++)
    {
        /*
         * if TCG mapping is supported,
         * map all TCs to TCG 0 - user can change it using BCM API.
         * must for 1 and 2 priorities ports
         *
         * For port priority propagation V1 -
         * all TCs are mapped to TCG 0
         *
         * For port priority propagation V2 -
         * each tc is mapped 1:1 to the same tcg
         *
         * For devices that doesn't support tcg_mapping -
         * each tc is mapped 1:1 to the same tcg
         */
        int tcg;
        if ((priority_propagation_enable && priority_propagation_version == DNX_SCHEDULER_PORT_PRIORITY_PROPAGATION_V2)
            || !(dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_tcg_mapping)))
        {
            tcg = tc;
        }
        else
        {
            tcg = 0;
        }
        SHR_IF_ERR_EXIT(dnx_sch_port_tc_to_tcg_map_set(unit, logical_port, tc, tcg));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get maximal value of max burst for TC / TCG shapers of the port
 */
int
dnx_scheduler_port_shaper_maximal_max_burst_get(
    int unit,
    int is_tcg,
    uint32 *max_burst)
{

    uint32 max_burst_max_hw;
    dbal_tables_e table = is_tcg ? DBAL_TABLE_SCH_PORT_TCG_SHAPER : DBAL_TABLE_SCH_PORT_TC_SHAPER;

    SHR_FUNC_INIT_VARS(unit);

    /** Get maximal value of the max_burst field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, table, DBAL_FIELD_MAX_BURST, 0, 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &max_burst_max_hw));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, max_burst_max_hw, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set TCG bandwidth of 1 or 2 priority port to maximum.
 * TC bandwidth for all ports and TCG bandwidth for 4 and 8 priorities ports are set in tune
 */
int
dnx_scheduler_port_shapers_bandwidth_init(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core;
    int nof_priorities;
    int tcg;
    uint32 max_quanta_to_add;

    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    if (!priority_propagation_enable && nof_priorities < dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {
        /** Get maximal value of quanta_to_add field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              DBAL_TABLE_SCH_PORT_TCG_SHAPER,
                                                              DBAL_FIELD_QUANTA_TO_ADD, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &max_quanta_to_add));

        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

        for (tcg = 0; tcg < nof_priorities; tcg++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_set(unit, core, base_hr + tcg, max_quanta_to_add));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Reset TCG bandwidth of 1 or 2 priority port to default value.
 */
int
dnx_scheduler_port_shapers_bandwidth_reset(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core;
    int nof_priorities;
    int tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    if (nof_priorities < dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

        for (tcg = 0; tcg < nof_priorities; tcg++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_reset(unit, core, base_hr + tcg));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set max burst of TC and TCG shapers of the port to default value
 */
int
dnx_scheduler_port_shapers_max_burst_reset(
    int unit,
    bcm_port_t logical_port)
{
    int nof_priorities;
    int tc, tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    for (tc = 0; tc < nof_priorities; tc++)
    {
        /** Set Port-TC max burst to maximum */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_max_burst_reset(unit, logical_port, tc));
    }
    for (tcg = 0; tcg < nof_priorities; tcg++)
    {
        /** Set Port-TCG max burst to maximum */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_max_burst_reset(unit, logical_port, tcg));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * allocate and configure sch interace for a port
 */
int
dnx_scheduler_interface_allocate(
    int unit,
    bcm_port_t port)
{
    int core;
    bcm_port_t master_port;
    dnx_algo_port_info_s port_info;
    dnx_scheduler_allocators_interface_type_t alloc_type;
    int e2e_if;
    int is_channelized;
    int nof_interfaces;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** get master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_SCH, &master_port));

    if (port == master_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));

        /** Try to allocate big calendars for ILKN interface */
        alloc_type = (!is_channelized ? DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_NON_CHANNELIZED :
                      (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0) && !priority_propagation_enable ?
                       DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG :
                       DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_SMALL));

        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_allocate(unit, core, alloc_type, nof_interfaces, &e2e_if));

        /** set e2e interface to port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));

    }
    else
    {
        /** get sch interface of master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, master_port, &e2e_if));
        if (!DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, e2e_if))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d belongs to non channelized interface. Non channelized interface can have only one port\n",
                         port);

        }
        /** set e2e interface to port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * deallocate and configure sch interace for a port
 */
int
dnx_scheduler_interface_deallocate(
    int unit,
    bcm_port_t port)
{
    int e2e_if;
    bcm_port_t next_master_port;
    int core;
    int nof_interfaces;

    SHR_FUNC_INIT_VARS(unit);

    /** deallocate  interface if this is the last port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_SCH,
                                             &next_master_port));

    if (next_master_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

        /** mark e2e interface as free */
        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_deallocate(unit, core, e2e_if, nof_interfaces));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * enable/disable interface of the provided port
 */
int
dnx_scheduler_port_interface_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int e2e_if;
    bcm_core_t core;
    int nof_interfaces;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

    for (i = 0; i < nof_interfaces; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_interface_enable_set(unit, core, e2e_if + i, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure priority/ies for the interface(s) of the provided port
 */
int
dnx_scheduler_port_interface_priority_set(
    int unit,
    bcm_port_t port)
{
    int nof_interfaces;
    int i;
    int e2e_if;
    bcm_core_t core;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    table_id = DNX_SCH_IF_SHAPER_DBAL_TABLE_GET(unit, e2e_if);
    for (i = 0; i < nof_interfaces; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_interface_priority_set(unit, core, table_id, e2e_if + i,
                                                            (nof_interfaces - i) * 8 / nof_interfaces - 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * clear priority/ies for the interface(s) of the provided port
 */
int
dnx_scheduler_port_interface_priority_clear(
    int unit,
    bcm_port_t port)
{
    int nof_interfaces;
    int i;
    int e2e_if;
    bcm_core_t core;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    table_id = DNX_SCH_IF_SHAPER_DBAL_TABLE_GET(unit, e2e_if);
    for (i = 0; i < nof_interfaces; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_interface_priority_clear(unit, core, table_id, e2e_if + i));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * map between SCH interface(s) and EGQ interfaces for the provided port
 */
int
dnx_scheduler_port_egq_to_sch_if_map(
    int unit,
    bcm_port_t port)
{
    int e2e_if, egq_if;
    int i;
    int nof_interfaces;
    bcm_core_t core;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get sch interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    /** get egq interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        /** configure sch if to egress if mapping */
        for (i = 0; i < nof_interfaces; i++)
        {
            /*
             * By default half of the sch interfaces will be mapped to high priority and half to low priority.
             * If single priority, map just the low priority to scheduler interface.
             */
            SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_set(unit, core, e2e_if + i, egq_if,
                                                                  (i < (nof_interfaces / 2) ? TRUE : FALSE)));
        }
    }
    else
    {
        /*
         * In case of RCY port, disable interface level FC from EGQ to SCH.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            e2e_if = dnx_data_sch.interface.reserved_get(unit);
        }

        /** configure egress if to sch if mapping */
        SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if, e2e_if));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * disable interface level FC from EGQ to SCH for the provided port
 */
int
dnx_scheduler_port_egq_to_sch_if_fc_disable(
    int unit,
    bcm_port_t port)
{
    int e2e_if, egq_if;
    int i;
    int nof_interfaces;
    bcm_core_t core;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        /** get sch interface from port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

        /** configure sch if to egress if mapping */
        for (i = 0; i < nof_interfaces; i++)
        {
            /*
             * Map sch interface to dummy EGQ interface
             */
            SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_set(unit, core, e2e_if + i,
                                                                  dnx_data_egr_queuing.params.reserved_if_get(unit),
                                                                  FALSE));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /** get egq interface from port mgmt */
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

            /** map egress if to reserved sch if */
            SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if,
                                                             dnx_data_sch.interface.reserved_get(unit)));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * unmap between SCH interface(s) and EGQ interfaces for the provided port
 */
int
dnx_scheduler_port_egq_to_sch_if_unmap(
    int unit,
    bcm_port_t port)
{
    int e2e_if, egq_if;
    int i;
    int nof_interfaces;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /** get sch interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    /** get egq interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        /** clear sch if to egress if mapping */
        for (i = 0; i < nof_interfaces; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_clear(unit, core, e2e_if + i));
        }
    }
    else
    {
        /** clear egress if to sch if mapping */
        SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_clear(unit, core, egq_if));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map port to interface for non channelized calendar
 */
int
dnx_scheduler_port_to_interface_map_set(
    int unit,
    bcm_port_t port)
{
    int e2e_if;
    int base_hr;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    if (!DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, e2e_if))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));
        SHR_IF_ERR_EXIT(dnx_sch_dbal_port_to_interface_map_set(unit, core, e2e_if, base_hr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unmap port to interface for non channelized calendar
 */
int
dnx_scheduler_port_to_interface_unmap_set(
    int unit,
    bcm_port_t port)
{
    int e2e_if;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    if (!DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, e2e_if))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_sch_dbal_port_to_interface_map_clear(unit, core, e2e_if));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure scheduler interface calendar traversal speed.
 * it is calculted from total calendar rate
 */
static int
dnx_scheduler_port_calendar_speed_calc(
    int unit,
    uint64 rate,
    uint32 *calendar_speed)
{
    uint32 rate_internal, calendar_speed_tmp, calendar_speed_max_value;
    uint32 credit_worth;
    uint64 calc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_u64_div_long_and_round_up
                    (unit, rate, dnx_data_sch.interface.cal_speed_resolution_get(unit), &calc));
    SHR_IF_ERR_EXIT(dnx_algo_cal_u64_to_long(unit, calc, &rate_internal));
    if (0 == rate_internal)
    {
        *calendar_speed = 0;
    }
    else
    {

        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate_internal, credit_worth, &calendar_speed_tmp));

        /** Get maximal value of calendar speed field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION,
                                                              DBAL_FIELD_CALENDAR_SPEED, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &calendar_speed_max_value));

        *calendar_speed = UTILEX_RANGE_BOUND(calendar_speed_tmp,
                                             dnx_data_sch.interface.cal_speed_resolution_get(unit),
                                             calendar_speed_max_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return total inteface calendar rate.
 * it is calculted from scheduler interface calendar traversal speed. 
 */
static int
dnx_scheduler_port_total_calendar_rate_get(
    int unit,
    int core,
    uint32 sch_interface,
    uint64 *rate)
{
    uint32 rate_internal, calendar_speed, credit_worth;
    uint64 calc_rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_get(unit, core, sch_interface, &calendar_speed));

    if (calendar_speed == 0)
    {
        COMPILER_64_ZERO(*rate);
    }
    else
    {
        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, calendar_speed, credit_worth, &rate_internal));
        COMPILER_64_SET(calc_rate, 0, rate_internal);
        COMPILER_64_UMUL_32(calc_rate, dnx_data_sch.interface.cal_speed_resolution_get(unit));

        COMPILER_64_COPY(*rate, calc_rate);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get active calendar id for the provided scheduler interface
 */
static int
dnx_scheduler_interface_active_calendar_get(
    int unit,
    int core,
    int sch_if,
    int *cal_length,
    uint32 *calendar)
{
    int active_cal;

    SHR_FUNC_INIT_VARS(unit);

    *cal_length = 0;

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_get(unit, core, sch_if, &active_cal, cal_length));

    /** Read active SCH calendar */
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_get(unit, core, sch_if, active_cal, *cal_length, calendar));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure new calendar for the provided scheduler interface
 */
static int
dnx_scheduler_interface_active_calendar_config(
    int unit,
    int core,
    uint32 sch_if,
    int nof_interfaces,
    uint32 cal_length,
    uint32 *calendar)
{
    int active_cal;
    int next_active_cal;
    int other_cal_length;

    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < nof_interfaces; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_get(unit, core, sch_if + i, &active_cal, &other_cal_length));

        next_active_cal = (active_cal + 1) % 2;

        /** Write non-active SCH calendar */
        SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_set(unit, core, sch_if, i, next_active_cal, cal_length, calendar));

        /** update the calendar length and set the calendar to be active */
        SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_set(unit, core, sch_if + i, next_active_cal, cal_length));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port rate from scheduler interface calendar
 */
int
dnx_scheduler_port_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *rate)
{
    uint64 total_cal_rate;
    int sch_cal_len, sch_if;
    bcm_core_t core;

    uint32 sch_cal[DNX_DATA_MAX_SCH_INTERFACE_BIG_CALENDAR_SIZE];
    int base_hr;

    int is_channelized_calendar;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_if);

    sch_cal_len = 0;

    if (is_channelized_calendar)
    {
        /** Retrieve calendars */
        SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_get(unit, core, sch_if, &sch_cal_len, sch_cal));

        /** Retrieve total sch rate*/
        SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_if, &total_cal_rate));

        /** Calculate sch rates*/
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

        SHR_IF_ERR_EXIT(dnx_algo_cal_simple_object_rate_get(unit, sch_cal, sch_cal_len, total_cal_rate, base_hr, rate));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_interface_shaper_rate_get(unit, logical_port, rate));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set port rate to swstate for scheduler interface calendar
 */
int
dnx_scheduler_port_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate)
{
    bcm_core_t core;
    int base_hr;
    int sch_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** Setting rate to software */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.rate.set(unit, core, base_hr, rate));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.valid.set(unit, core, base_hr, TRUE));

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));

    /** mark E2E interface as modified */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_set(unit, core, sch_if));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure scheduler interface calendar for the specified port
 */
int
dnx_scheduler_port_single_if_rate_hw_set(
    int unit,
    bcm_port_t port,
    int is_remove)
{
    uint32 sch_calendar_len, max_cal_size;
    uint64 current_if_rate = 0;
    uint64 total_if_rate = 0;
    int sch_offset, sch_offset_i;

    int rate;
    uint32 rates_per_hr[DNX_DATA_MAX_SCH_FLOW_NOF_HR];
    uint32 calendar[DNX_DATA_MAX_SCH_INTERFACE_BIG_CALENDAR_SIZE];

    int is_valid = FALSE;
    int core;
    uint32 is_rate_decreased = FALSE;
    bcm_pbmp_t ifs_bmp;
    bcm_port_t port_i;
    int base_hr;
    int nof_interfaces;

    dbal_tables_e table_id;
    int entry_max_value;

    int is_channelized_calendar;

    uint32 calendar_speed = 0;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** Get scheduler offset */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &sch_offset));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    sal_memset(rates_per_hr, 0, sizeof(uint32) * dnx_data_sch.flow.nof_hr_get(unit));
    sal_memset(calendar, 0, sizeof(uint32) * dnx_data_sch.interface.big_calendar_size_get(unit));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_offset);

    if (is_channelized_calendar)
    {
        COMPILER_64_ZERO(total_if_rate);

        table_id = DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_offset);

        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &ifs_bmp));

        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));

        if (priority_propagation_enable && dnx_data_sch.general.port_priority_propagation_version_get(unit)
            == DNX_SCHEDULER_PORT_PRIORITY_PROPAGATION_V1)
        {
            /** build calendar as a simple port list */

            sch_calendar_len = 0;
            BCM_PBMP_ITER(ifs_bmp, port_i)
            {
                if (is_remove && port_i == port)
                {
                    /** skip the port being removed */
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port_i, &sch_offset_i));
                if (sch_offset == sch_offset_i)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port_i, &base_hr));
                    calendar[sch_calendar_len] = base_hr;
                    sch_calendar_len++;
                }
            }
            if (sch_calendar_len == 0)
            {
                /** Calendar length should be at least 1 */
                sch_calendar_len = 1;
            }
            calendar_speed = dnx_data_sch.interface.cal_speed_resolution_get(unit);
        }
        else
        {
            /** Retrive rates from the software database*/
            BCM_PBMP_ITER(ifs_bmp, port_i)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port_i, &sch_offset_i));
                if (sch_offset == sch_offset_i)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port_i, &base_hr));
                    SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.valid.get(unit, core, base_hr, &is_valid));
                    if (is_valid)
                    {
                        SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.rate.get(unit, core, base_hr, &rate));

                        rates_per_hr[base_hr] = rate;
                        COMPILER_64_ADD_32(total_if_rate, rates_per_hr[base_hr]);
                    }
                }
            }

            /** Get maximal value of calendar entry key field */
            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, table_id, DBAL_FIELD_ENTRY, TRUE, 0, 0,
                                                            &entry_max_value));

            max_cal_size = entry_max_value + 1;

            /** Calculate calendar*/
            SHR_IF_ERR_EXIT(dnx_algo_cal_simple_from_rates_to_calendar(unit, rates_per_hr,
                                                                       dnx_data_sch.flow.nof_hr_get(unit),
                                                                       total_if_rate, max_cal_size,
                                                                       calendar, &sch_calendar_len));

            SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_offset, &current_if_rate));
            SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_speed_calc(unit, total_if_rate, &calendar_speed));

            /** is rate decreased? */
            if (COMPILER_64_GT(current_if_rate, total_if_rate))
            {
                is_rate_decreased = TRUE;
            }

        }

        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

        if (!is_rate_decreased)
        {
            /** Configure traversal rate*/
            SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_set(unit, core, sch_offset, nof_interfaces,
                                                                 calendar_speed));
        }

        /** Configure calendar */
        SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_config(unit, core, sch_offset, nof_interfaces,
                                                                       sch_calendar_len, calendar));

        if (is_rate_decreased)
        {
            /** Configure traversal rate*/
            SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_set(unit, core, sch_offset, nof_interfaces,
                                                                 calendar_speed));
        }
    }
    else
    {
        /** non channelized calendar */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));
        SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.valid.get(unit, core, base_hr, &is_valid));
        if (is_valid)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_db.port_shaper.rate.get(unit, core, base_hr, &rate));

            /** For non-channalized interfaces - port rate is same as if rate*/
            SHR_IF_ERR_EXIT(dnx_scheduler_interface_shaper_rate_set(unit, port, rate));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure all modified scheduler interface calendars
 */
int
dnx_scheduler_port_rate_hw_set(
    int unit)
{
    int sch_offset;

    uint8 is_modified = FALSE;
    int core;
    bcm_pbmp_t ports_bmp;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0, &ports_bmp));

        BCM_PBMP_ITER(ports_bmp, port)
        {
            /** Get scheduler offset */
            SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &sch_offset));

            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_get(unit, core, sch_offset, &is_modified));
            if (!is_modified)
            {
                continue;
            }

            /** mark E2E interface as not modified */
            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_clear(unit, core, sch_offset));

            /** configure the rate */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_single_if_rate_hw_set(unit, port, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set interface shaper bandwidth
 */
int
dnx_scheduler_interface_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate)
{
    bcm_core_t core;
    uint32 credit_rate, credit_worth;
    int sch_interface;
    uint32 rate_max_value;
    int is_channelized_calendar;
    uint32 shaper_resolution;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_interface);
    shaper_resolution = dnx_data_sch.interface.shaper_resolution_get(unit, is_channelized_calendar)->resolution;

    /** Get credit size */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    table_id = (is_channelized_calendar ? DBAL_TABLE_SCH_IF_SHAPER : DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER);

    /** Get Device Interface Max Credit Rate */
    if (0 == rate)
    {
        credit_rate = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate, credit_worth, &credit_rate));

        credit_rate = credit_rate * shaper_resolution;

        /** Get maximal value of interface credit rate field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, table_id,
                                                              DBAL_FIELD_CREDIT_RATE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &rate_max_value));
        credit_rate = UTILEX_RANGE_BOUND(credit_rate, shaper_resolution, rate_max_value);
    }

    SHR_IF_ERR_EXIT(dnx_sch_interface_shaper_bandwidth_hw_set(unit, core, table_id, sch_interface, credit_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get interface shaper bandwidth
 */
int
dnx_scheduler_interface_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *if_rate)
{
    bcm_core_t core;
    uint32 rate_internal, credit_rate, credit_worth;
    int sch_interface;
    int is_channelized_calendar;
    uint32 shaper_resolution;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_interface);
    shaper_resolution = dnx_data_sch.interface.shaper_resolution_get(unit, is_channelized_calendar)->resolution;

    /** Get credit size */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    table_id = (is_channelized_calendar ? DBAL_TABLE_SCH_IF_SHAPER : DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER);

    /** Get Sch Interface Max Credit Rate */
    SHR_IF_ERR_EXIT(dnx_sch_interface_shaper_bandwidth_hw_get(unit, core, table_id, sch_interface, &credit_rate));

    if (0 == credit_rate)
    {
        rate_internal = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, credit_rate, credit_worth, &rate_internal));

        rate_internal = rate_internal * shaper_resolution;
    }

    *if_rate = rate_internal;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See scheduler_port.h
 */
int
dnx_scheduler_port_tcg_weight_set(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int is_valid,
    int weight)
{

    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** Get port core, base HR and nof priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    /** Force flow control on HR before setting TCG weight.*/
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, TRUE));

    /** Set TCG weight */
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_set(unit, core, base_hr, cosq, is_valid, weight));

    /** Disable flow control on HR after setting TCG weight value.*/
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See scheduler_port.h
 */
int
dnx_scheduler_port_tcg_weight_get(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int *is_valid,
    int *weight)
{

    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** Get port core, base HR and nof priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    /** Get TCG weight */
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_get(unit, core, base_hr, cosq, is_valid, weight));

exit:
    SHR_FUNC_EXIT;
}
