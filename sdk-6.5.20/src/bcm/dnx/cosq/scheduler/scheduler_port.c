


/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
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
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

#include "scheduler_dbal.h"
#include "scheduler_element.h"
#include "scheduler_port.h"
#include "scheduler_allocators.h"

#define DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_if) ((sch_if) < dnx_data_sch.interface.nof_channelized_calendars_get(unit))


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


static shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 rate,                
    uint32 credit_div,          
    uint32 *quanta              
    )
{
    uint32 calc2, calc;
    uint32 credit_worth;

    uint32 ticks_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == rate)
    {
        *quanta = 0;
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        
        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        
        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        

        calc2 = (calc == 0 ? 0 : UTILEX_DIV_ROUND_UP(rate, calc));

        *quanta = (calc2 == 0 ? 1 : calc2);

    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 rate,                
    uint32 *quanta              
    )
{
    uint32 divider;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &divider));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(unit, port_calendar_type, rate, divider,
                                                                                quanta));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_port_quanta_to_rate_kbits_per_sec(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 quanta,              
    uint32 *rate                
    )
{
    UTILEX_U64 calc2;
    uint32 calc, tmp;
    uint32 credit_worth;
    uint32 ticks_per_sec;
    uint32 credit_div;          

    SHR_FUNC_INIT_VARS(unit);

    if (0 == quanta)
    {
        *rate = 0;
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

        
        
        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        utilex_u64_multiply_longs(calc, quanta, &calc2);
        if (utilex_u64_to_long(&calc2, &tmp))
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Overflow when calculating rate\n");
        }

        *rate = tmp;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_scheduler_port_shaper_calculate_divider(
    int unit,
    uint32 *divider)
{
    uint32 new_quanta, curr_divider, iterate_divider;

    uint32 max_rate;
    uint32 max_divider;
    uint32 max_quanta_to_add;

    SHR_FUNC_INIT_VARS(unit);

    max_rate = dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000; 

    
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_SCH_PORT_CREDIT_CONFIGURATION,
                                                          DBAL_FIELD_CREDIT_WORTH, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                          &max_divider));

    
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


static shr_error_e
dnx_scheduler_port_shaper_max_burst_to_internal(
    int unit,
    uint32 max_burst,
    uint32 *internal)
{
    uint32 credit_worth, credit_div;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

    *internal = max_burst * credit_div / credit_worth;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_port_shaper_max_burst_from_internal(
    int unit,
    uint32 internal,
    uint32 *max_burst)
{
    uint32 credit_worth, credit_div;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

    *max_burst = internal * credit_worth / credit_div;

exit:
    SHR_FUNC_EXIT;
}


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


shr_error_e
dnx_scheduler_port_shaper_calendar_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_config_init(unit));

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_credit_divider_init(unit));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TC_CALENDAR, rate, &quanta));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_bandwidth_set(unit, core, base_hr + priority_ndx, quanta));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TC_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TCG_CALENDAR, rate, &quanta));

    global_tcg = base_hr + tcg_ndx;

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_set(unit, core, global_tcg, quanta));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TCG_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_to_internal(unit, max_burst, &internal));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_set(unit, core, base_hr + port_priority, internal));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, internal, max_burst));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_to_internal(unit, max_burst, &internal));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_set(unit, core, base_hr + tcg_index, internal));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, internal, max_burst));
exit:
    SHR_FUNC_EXIT;
}


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

    
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr);

    res = cosq_config.hr_group_bw.get(unit, core, base_hr, &hr_rate);
    SHR_IF_ERR_EXIT(res);

    
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

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_min_bw_group_get(unit, core, group));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.get(unit, core, *group, &group_rate));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.set(unit, core, *group, (group_rate + interface_max_rate)));
    SHR_IF_ERR_EXIT(cosq_config.hr_group_bw.set(unit, core, base_hr, interface_max_rate));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core, se_id, group));

    
    SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, TRUE));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core, se_id, group));
exit:
    SHR_FUNC_EXIT;
}


int
dnx_scheduler_port_hr_color_decide_and_set(
    int unit,
    bcm_port_t logical_port)
{
    int group;
    int priority_i;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_group_calc(unit, logical_port, &group));      

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
        
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_group_set(unit, logical_port, priority_i, group));
    }

exit:
    SHR_FUNC_EXIT;
}


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
        
        SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, FALSE));

        
    }

exit:
    SHR_FUNC_EXIT;
}


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
        
        SHR_IF_ERR_EXIT(dnx_sch_port_priorities_nof_clear(unit, core_id, base_hr));
    }

exit:
    SHR_FUNC_EXIT;
}


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


int
dnx_scheduler_port_tc_to_tcg_map_init(
    int unit,
    bcm_port_t logical_port)
{
    int nof_priorities;
    int tc;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    for (tc = 0; tc < nof_priorities; tc++)
    {
        
        SHR_IF_ERR_EXIT(dnx_sch_port_tc_to_tcg_map_set(unit, logical_port, tc, (priority_propagation_enable ? tc : 0)));
    }

exit:
    SHR_FUNC_EXIT;
}


int
dnx_scheduler_port_shaper_maximal_max_burst_get(
    int unit,
    int is_tcg,
    uint32 *max_burst)
{

    uint32 max_burst_max_hw;
    dbal_tables_e table = is_tcg ? DBAL_TABLE_SCH_PORT_TCG_SHAPER : DBAL_TABLE_SCH_PORT_TC_SHAPER;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, table, DBAL_FIELD_MAX_BURST, 0, 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &max_burst_max_hw));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_max_burst_from_internal(unit, max_burst_max_hw, max_burst));

exit:
    SHR_FUNC_EXIT;
}


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
        
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_max_burst_reset(unit, logical_port, tc));
    }
    for (tcg = 0; tcg < nof_priorities; tcg++)
    {
        
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_max_burst_reset(unit, logical_port, tcg));
    }

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_SCH, &master_port));

    if (port == master_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));

        
        alloc_type = (!is_channelized ? DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_NON_CHANNELIZED :
                      (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0) && !priority_propagation_enable ?
                       DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_BIG :
                       DNX_SCHEDULER_ALLOCATORS_INTERFACE_TYPE_CHANNELIZED_SMALL));

        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_allocate(unit, core, alloc_type, nof_interfaces, &e2e_if));

        
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));

    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, master_port, &e2e_if));
        if (!DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, e2e_if))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Port %d belongs to non channelized interface. Non channelized interface can have only one port\n",
                         port);

        }
        
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));
    }

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_SCH,
                                             &next_master_port));

    if (next_master_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

        
        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_deallocate(unit, core, e2e_if, nof_interfaces));

    }

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        
        for (i = 0; i < nof_interfaces; i++)
        {
            
            SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_set(unit, core, e2e_if + i, egq_if,
                                                                  (i < (nof_interfaces / 2) ? TRUE : FALSE)));
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            e2e_if = dnx_data_sch.interface.reserved_get(unit);
        }

        
        SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if, e2e_if));
    }
exit:
    SHR_FUNC_EXIT;
}


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
        
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

        
        for (i = 0; i < nof_interfaces; i++)
        {
            
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
            
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

            
            SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if,
                                                             dnx_data_sch.interface.reserved_get(unit)));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        
        for (i = 0; i < nof_interfaces; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_clear(unit, core, e2e_if + i));
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_clear(unit, core, egq_if));
    }
exit:
    SHR_FUNC_EXIT;
}


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


static int
dnx_scheduler_port_total_calendar_rate_set(
    int unit,
    int core,
    int sch_interface,
    int nof_interfaces,
    uint32 rate)
{
    uint32 rate_internal, calendar_speed, calendar_speed_max_value;
    uint32 credit_worth;

    SHR_FUNC_INIT_VARS(unit);

    

    
    rate_internal = UTILEX_DIV_ROUND_UP(rate, dnx_data_sch.interface.cal_speed_resolution_get(unit));

    if (0 == rate_internal)
    {
        calendar_speed = 0;
    }
    else
    {

        
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate_internal, credit_worth, &calendar_speed));

        
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION,
                                                              DBAL_FIELD_CALENDAR_SPEED, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &calendar_speed_max_value));

        calendar_speed = UTILEX_RANGE_BOUND(calendar_speed,
                                            dnx_data_sch.interface.cal_speed_resolution_get(unit),
                                            calendar_speed_max_value);
    }

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_set(unit, core, sch_interface, nof_interfaces, calendar_speed));

exit:
    SHR_FUNC_EXIT;
}


static int
dnx_scheduler_port_total_calendar_rate_get(
    int unit,
    int core,
    uint32 sch_interface,
    uint32 *rate)
{
    uint32 rate_internal, calendar_speed, credit_worth;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_get(unit, core, sch_interface, &calendar_speed));

    if (calendar_speed == 0)
    {
        *rate = 0;
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, calendar_speed, credit_worth, &rate_internal));

        *rate = rate_internal * dnx_data_sch.interface.cal_speed_resolution_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_get(unit, core, sch_if, active_cal, *cal_length, calendar));

exit:
    SHR_FUNC_EXIT;
}


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

        
        SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_set(unit, core, sch_if, i, next_active_cal, cal_length, calendar));

        
        SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_set(unit, core, sch_if + i, next_active_cal, cal_length));
    }

exit:
    SHR_FUNC_EXIT;
}


int
dnx_scheduler_port_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *rate)
{
    uint32 total_cal_rate;
    int sch_cal_len, sch_if;
    bcm_core_t core;

    uint32 sch_cal[DNX_DATA_MAX_SCH_INTERFACE_BIG_CALENDAR_SIZE];
    int base_hr;

    int is_channelized_calendar;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_if);

    sch_cal_len = 0;

    if (is_channelized_calendar)
    {
        
        SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_get(unit, core, sch_if, &sch_cal_len, sch_cal));

        
        SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_if, &total_cal_rate));

        
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


int
dnx_scheduler_port_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate)
{
    bcm_core_t core;
    int sch_if;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.set(unit, logical_port, rate));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.set(unit, logical_port, TRUE));

    
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    
    SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_set(unit, core, sch_if));

exit:
    SHR_FUNC_EXIT;
}


int
dnx_scheduler_port_rate_hw_set(
    int unit)
{
    uint32 sch_calendar_len, max_cal_size;
    uint32 total_if_rate = 0, current_if_rate = 0;
    int sch_offset, sch_offset_i;

    int rate;
    uint32 rates_per_hr[DNX_DATA_MAX_SCH_FLOW_NOF_HR];
    uint32 calendar[DNX_DATA_MAX_SCH_INTERFACE_BIG_CALENDAR_SIZE];

    int is_valid = FALSE;
    uint8 is_modified = FALSE;
    int core;
    uint32 is_rate_decreased = FALSE;
    bcm_pbmp_t ifs_bmp, ports_bmp;
    bcm_port_t port, port_i;
    int base_hr;
    int nof_interfaces;

    dbal_tables_e table_id;
    int entry_max_value;

    int is_channelized_calendar;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0, &ports_bmp));

        BCM_PBMP_ITER(ports_bmp, port)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &sch_offset));

            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_get(unit, core, sch_offset, &is_modified));
            if (!is_modified)
            {
                continue;
            }

            sal_memset(rates_per_hr, 0, sizeof(uint32) * dnx_data_sch.flow.nof_hr_get(unit));
            sal_memset(calendar, 0, sizeof(uint32) * dnx_data_sch.interface.big_calendar_size_get(unit));

            
            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_clear(unit, core, sch_offset));

            is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_offset);

            if (is_channelized_calendar)
            {
                dnx_algo_port_info_s port_info;
                total_if_rate = 0;

                SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &ifs_bmp));

                
                BCM_PBMP_ITER(ifs_bmp, port_i)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port_i, &port_info));
                    if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
                    {
                        continue;
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port_i, &sch_offset_i));
                    if (sch_offset == sch_offset_i)
                    {
                        SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.get(unit, port_i, &is_valid));
                        if (is_valid)
                        {
                            SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.get(unit, port_i, &rate));
                            SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port_i, &base_hr));

                            rates_per_hr[base_hr] = rate;
                            total_if_rate += rates_per_hr[base_hr];
                        }
                    }
                }

                
                SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_offset, &current_if_rate));
                SHR_IF_ERR_EXIT(dnx_scheduler_port_nof_interfaces_get(unit, port, &nof_interfaces));

                
                if (current_if_rate > total_if_rate)
                {
                    is_rate_decreased = TRUE;
                }

                if (!is_rate_decreased)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_set(unit, core, sch_offset, nof_interfaces,
                                                                               total_if_rate));
                }

                table_id = DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_offset);

                
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, table_id, DBAL_FIELD_ENTRY, TRUE, 0, 0,
                                                                &entry_max_value));

                max_cal_size = entry_max_value + 1;

                
                SHR_IF_ERR_EXIT(dnx_algo_cal_simple_from_rates_to_calendar(unit, rates_per_hr,
                                                                           dnx_data_sch.flow.nof_hr_get(unit),
                                                                           total_if_rate, max_cal_size,
                                                                           calendar, &sch_calendar_len));

                
                SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_config(unit, core, sch_offset, nof_interfaces,
                                                                               sch_calendar_len, calendar));

                if (is_rate_decreased)
                {
                    
                    SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_set(unit, core, sch_offset, nof_interfaces,
                                                                               total_if_rate));
                }
            }
            else
            {
                
                SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.get(unit, port, &is_valid));
                if (is_valid)
                {
                    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.get(unit, port, &rate));

                    
                    SHR_IF_ERR_EXIT(dnx_scheduler_interface_shaper_rate_set(unit, port, rate));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


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

    

    
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    is_channelized_calendar = DNX_SCHEDULER_PORT_IS_CHANNELIZED_CALENDAR(unit, sch_interface);
    shaper_resolution = dnx_data_sch.interface.shaper_resolution_get(unit, is_channelized_calendar)->resolution;

    
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    table_id = (is_channelized_calendar ? DBAL_TABLE_SCH_IF_SHAPER : DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER);

    
    if (0 == rate)
    {
        credit_rate = 0;
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate, credit_worth, &credit_rate));

        

        
        credit_rate = credit_rate * shaper_resolution;

        
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, table_id,
                                                              DBAL_FIELD_CREDIT_RATE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &rate_max_value));
        credit_rate = UTILEX_RANGE_BOUND(credit_rate, shaper_resolution, rate_max_value);
    }

    SHR_IF_ERR_EXIT(dnx_sch_interface_shaper_bandwidth_hw_set(unit, core, table_id, sch_interface, credit_rate));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    table_id = (is_channelized_calendar ? DBAL_TABLE_SCH_IF_SHAPER : DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER);

    
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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, TRUE));

    
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_set(unit, core, base_hr, cosq, is_valid, weight));

    
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, FALSE));

exit:
    SHR_FUNC_EXIT;
}


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

    
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_get(unit, core, base_hr, cosq, is_valid, weight));

exit:
    SHR_FUNC_EXIT;
}
