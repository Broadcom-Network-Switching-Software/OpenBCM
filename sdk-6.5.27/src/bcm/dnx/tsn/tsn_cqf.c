
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_tsn_types.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm/tsn.h>

#include "tsn_common.h"
#include "tsn_counter.h"
#include "tsn_cqf.h"
#include "tsn_dbal.h"

#define DNX_TSN_CQF_GATE_ALLOC "TSN CQF GATE ALLOC"
#define CQF_PORT_UNUSED (-1)

static shr_error_e
dnx_tsn_cqf_alloc_mng_init(
    int unit)
{
    dnx_algo_res_create_data_t gate_alloc_create_data;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.gate_alloc.alloc(unit));

    sal_memset(&gate_alloc_create_data, 0, sizeof(gate_alloc_create_data));
    gate_alloc_create_data.first_element = 0;
    gate_alloc_create_data.nof_elements = dnx_data_tsn.cqf.nof_cqf_ports_get(unit);
    sal_strncpy(gate_alloc_create_data.name, DNX_TSN_CQF_GATE_ALLOC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.gate_alloc.create(unit, core_id, &gate_alloc_create_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_sw_init(
    int unit)
{
    bcm_core_t core_id;
    int egr_tsn_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_alloc_mng_init(unit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.ptp_base_time.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        for (egr_tsn_if = 0; egr_tsn_if < dnx_data_tsn.cqf.nof_cqf_ports_get(unit); egr_tsn_if++)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.set(unit, core_id, egr_tsn_if, CQF_PORT_UNUSED));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_port_control_list_gate_and_phase_init(
    int unit)
{
    int core_id;
    int next_phase;
    int next_gate_state;
    int idx, cqf_port, i;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        for (cqf_port = 0; cqf_port < dnx_data_tsn.cqf.nof_cqf_ports_get(unit); cqf_port++)
            for (i = 0; i < dnx_data_tsn.cqf.max_time_intervals_entries_get(unit); i++)
            {

                idx = (i + 1) % dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);
                next_gate_state = dnx_data_tsn.cqf.port_control_list_init_get(unit, idx)->gate_state;
                next_phase = dnx_data_tsn.cqf.port_control_list_init_get(unit, idx)->phase;

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_gate_and_phase_hw_init
                                (unit, core_id, cqf_port, i, next_gate_state, next_phase));
            }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_hw_init(
    int unit)
{
    int cqf_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_global_config_hw_init(unit));

    for (cqf_port = 0; cqf_port < dnx_data_tsn.cqf.nof_cqf_ports_get(unit); cqf_port++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_hw_init(unit, cqf_port));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_gate_and_phase_init(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_global_config_hw_enable(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_port_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    int is_channelized;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!(DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF support only NIF ports, port - %d", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
    if (is_channelized)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF supported only not channelized ports, port- %d", port);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));

    if (nof_priorities <= 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF supported only ports with 2 or more priorities, port- %d", port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_sch_prio_1_is_disabled_verify(
    int unit,
    bcm_port_t port)
{
    bcm_gport_t e2e_gport;
    int arg;

    SHR_FUNC_INIT_VARS(unit);

    BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_gport, port);

    SHR_IF_ERR_EXIT(dnx_scheduler_control_e2e_port_tc_get(unit, e2e_gport, 1, bcmCosqControlBandwidthBurstMax, &arg));

    if (arg != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF HR1 shaper is not disabled for port- %d", port);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_egq_prio_1_is_not_mapped_verify(
    int unit,
    bcm_port_t port)
{
    bcm_gport_t gport;
    bcm_cosq_queue_offset_mapping_key_t key;
    bcm_cosq_queue_offset_t offset;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_LOCAL_SET(gport, port);

    for (int priority = 0; priority < DNX_COSQ_NOF_TC; priority++)
    {
        for (int dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            key.tc = priority;
            key.dp = dp;
            SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_map_get(unit, gport, 1, &key, &offset));
            if (offset.cosq == 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF qpair 1 shouldn't be mapped to any system TC!");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_set_verify(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_tsn_cqf_config_t * config)
{

    int nof_sch_priorities;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_verify(unit, port));

    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "NO flags are currently supported. flags parameter must be set to 0!");

    SHR_BOOL_VERIFY(config->enable, _SHR_E_PARAM, "Only values 0 and 1 are supported for enable!\r\n");

    if ((config->ptp_base_time.nanoseconds % dnx_data_tsn.time.tick_period_ns_get(unit)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF base time needs to be a microseconds multiplier");
    }

    if ((config->start_tx <= 0) || (config->end_tx <= 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF start_tx and end_tx should be above 0");
    }

    if ((config->start_tx + config->end_tx) >= config->interval)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN CQF end_tx - start_tx is bigger than interval");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &nof_sch_priorities));
    if (nof_sch_priorities > 1)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_sch_prio_1_is_disabled_verify(unit, port));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_egq_prio_1_is_not_mapped_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_gate_get(
    int unit,
    int core_id,
    int egr_tsn_if,
    int allocate,
    int *gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, gate_id));
    if ((*gate_id == CQF_PORT_UNUSED) && allocate)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.gate_alloc.allocate_single(unit, core_id, 0, NULL, gate_id));
        SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.set(unit, core_id, egr_tsn_if, *gate_id));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_gate_free(
    int unit,
    int core_id,
    int egr_tsn_if)
{
    int gate_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    if (gate_id != CQF_PORT_UNUSED)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.gate_alloc.free_single(unit, core_id, gate_id, NULL));
        SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.tsn_egr_if_to_gate_mapping.set(unit, core_id, egr_tsn_if, CQF_PORT_UNUSED));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_base_time_sw_set(
    int unit,
    int core_id,
    int egr_tsn_if,
    bcm_ptp_timestamp_t ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.ptp_base_time.set(unit, core_id, egr_tsn_if, ptp_base_time));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_base_time_sw_get(
    int unit,
    int core_id,
    int egr_tsn_if,
    bcm_ptp_timestamp_t * ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.cqf.ptp_base_time.get(unit, core_id, egr_tsn_if, ptp_base_time));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_port_control_list_intervals_set(
    int unit,
    int core_id,
    int set,
    int gate_id,
    bcm_tsn_cqf_config_t * config)
{
    int nof_time_intervals = dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);
    uint32 actual_interval_time;
    uint64 num_tick_period = COMPILER_64_INIT(0, 0);
    int case_calc, ti_index;
    SHR_FUNC_INIT_VARS(unit);

    actual_interval_time = config->interval - (config->start_tx + config->end_tx);

    for (ti_index = 0; ti_index < nof_time_intervals; ti_index++)
    {
        case_calc = ti_index % 3;

        switch (case_calc)
        {
            case (0):
                COMPILER_64_SET(num_tick_period, 0, config->start_tx);
                break;
            case (1):
                COMPILER_64_SET(num_tick_period, 0, actual_interval_time);
                break;
            case (2):
                COMPILER_64_SET(num_tick_period, 0, config->end_tx);
                break;
        }

        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_set
                        (unit, core_id, set, gate_id, ti_index, num_tick_period));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_port_params_set(
    int unit,
    int core_id,
    int set,
    int gate_id,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    int base_q;
    int egr_tsn_if;
    uint64 base_time_in_tsn_counter;
    dbal_enum_value_field_egq_ps_mode_e ps_mode = 0;
    bcm_ptp_timestamp_t ptp_base_time_from_sw;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q));
    SHR_IF_ERR_EXIT(dnx_tsn_common_port_to_ps_mode_get(unit, port, &ps_mode));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_get(unit, core_id, egr_tsn_if, &ptp_base_time_from_sw));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter
                    (unit, &ptp_base_time_from_sw, &base_time_in_tsn_counter));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_hw_set
                    (unit, core_id, set, gate_id, base_time_in_tsn_counter, base_q, ps_mode));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_hw_set(
    int unit,
    int core_id,
    int gate_id,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_get(unit, core_id, &set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_active_hw_set(unit, core_id, !set, gate_id, 0));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_port_params_set(unit, core_id, set, gate_id, port, config));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_set(unit, core_id, set, gate_id, config));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_active_hw_set(unit, core_id, set, gate_id, 1));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_port_params_set(unit, core_id, !set, gate_id, port, config));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_set(unit, core_id, !set, gate_id, config));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_active_hw_set(unit, core_id, !set, gate_id, 1));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_port_control_list_intervals_clear(
    int unit,
    int core_id,
    int set,
    int gate_id)
{
    int nof_time_intervals = dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);
    int ti_index;

    SHR_FUNC_INIT_VARS(unit);

    for (ti_index = 0; ti_index < nof_time_intervals; ti_index++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_set
                        (unit, core_id, set, gate_id, ti_index, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_hw_clear(
    int unit,
    int core_id,
    int gate_id,
    bcm_gport_t port)
{
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_get(unit, core_id, &set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_active_hw_set(unit, core_id, !set, gate_id, 0));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_hw_clear(unit, core_id, set, gate_id));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_clear(unit, core_id, set, gate_id));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_hw_clear(unit, core_id, !set, gate_id));
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_clear(unit, core_id, !set, gate_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_cqf_config_set(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{

    int egr_tsn_if;
    int core_id;
    int gate_id;
    uint32 time_margin = dnx_data_tsn.cqf.time_margin_get(unit);
    bcm_ptp_timestamp_t config_ptp_base_time_updated;
    int is_mutex_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_cqf_config_set_verify(unit, flags, port, config));

    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    if (config->enable == 1)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_get(unit, core_id, egr_tsn_if, 1, &gate_id));
        SHR_IF_ERR_EXIT(dnx_tsn_counter_config_change_time_get
                        (unit, &config->ptp_base_time, config->interval, time_margin, &config_ptp_base_time_updated));
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_set(unit, core_id, egr_tsn_if, config_ptp_base_time_updated));
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_hw_set(unit, core_id, gate_id, port, config));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_get(unit, core_id, egr_tsn_if, 0, &gate_id));
        if (gate_id != CQF_PORT_UNUSED)
        {
            bcm_ptp_timestamp_t reset_base_time = { 0, 0 };
            SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_hw_clear(unit, core_id, gate_id, port));
            SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_free(unit, core_id, egr_tsn_if));
            SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_set(unit, core_id, egr_tsn_if, reset_base_time));
        }
    }

exit:
    if (is_mutex_locked)
    {

        SHR_IF_ERR_CONT(dnx_tsn_db.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_get_verify(
    int unit,
    uint32 flags,
    bcm_port_t port)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_verify(unit, port));

    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "NO flags are currently supported. flags parameter must be set to 0!");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_port_control_list_intervals_get(
    int unit,
    int core_id,
    int set,
    int gate_id,
    bcm_tsn_cqf_config_t * config)
{

    uint32 actual_interval_time = 0;
    uint64 num_tick_period = COMPILER_64_INIT(0, 0);
    int case_calc, ti_index;
    SHR_FUNC_INIT_VARS(unit);

    for (ti_index = 0; ti_index < 3; ti_index++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_get
                        (unit, core_id, set, gate_id, ti_index, &num_tick_period));

        case_calc = ti_index % 3;

        switch (case_calc)
        {
            case (0):
                config->start_tx = COMPILER_64_LO(num_tick_period);
                break;
            case (1):
                actual_interval_time = COMPILER_64_LO(num_tick_period);
                break;
            case (2):
                config->end_tx = COMPILER_64_LO(num_tick_period);
                break;
        }

    }

    config->interval = (config->start_tx + config->end_tx) + actual_interval_time;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_cqf_config_hw_get(
    int unit,
    int core_id,
    int gate_id,
    bcm_tsn_cqf_config_t * config)
{
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_get(unit, core_id, &set));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_get(unit, core_id, !set, gate_id, config));

    config->enable = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_cqf_config_get(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{

    int egr_tsn_if;
    int core_id;
    int gate_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_cqf_config_get_verify(unit, flags, port));

    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_get(unit, core_id, egr_tsn_if, 0, &gate_id));

    if (gate_id == CQF_PORT_UNUSED)
    {
        bcm_tsn_cqf_config_t_init(config);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_get(unit, core_id, egr_tsn_if, &config->ptp_base_time));

        SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_hw_get(unit, core_id, gate_id, config));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_cqf_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_sw_init(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_cqf_port_deactivate_all(
    int unit)
{
    bcm_core_t core_id;
    int egr_tsn_if;
    int gate_id;
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_get(unit, core_id, &set));

        for (egr_tsn_if = 0; egr_tsn_if < dnx_data_tsn.cqf.nof_cqf_ports_get(unit); egr_tsn_if++)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_get(unit, core_id, egr_tsn_if, 0, &gate_id));
            if (gate_id != CQF_PORT_UNUSED)
            {
                SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_active_hw_set(unit, core_id, !set, gate_id, 0));
            }
        }

        SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_cqf_port_base_time_restart(
    int unit)
{
    bcm_ptp_timestamp_t config_change_time;
    uint64 tsn_counter_base_time;
    bcm_tsn_cqf_config_t config;
    bcm_core_t core_id;
    int egr_tsn_if;
    int gate_id;
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_get(unit, core_id, &set));

        for (egr_tsn_if = 0; egr_tsn_if < dnx_data_tsn.cqf.nof_cqf_ports_get(unit); egr_tsn_if++)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_cqf_gate_get(unit, core_id, egr_tsn_if, 0, &gate_id));
            if (gate_id != CQF_PORT_UNUSED)
            {

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_get(unit, core_id, egr_tsn_if, &config.ptp_base_time));

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_control_list_intervals_get(unit, core_id, !set, gate_id, &config));

                SHR_IF_ERR_EXIT(dnx_tsn_counter_config_change_time_get
                                (unit, &config.ptp_base_time, config.interval, 1000000, &config_change_time));

                SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter
                                (unit, &config_change_time, &tsn_counter_base_time));

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_base_params_hw_set
                                (unit, core_id, !set, gate_id, tsn_counter_base_time, TRUE));

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));
                SHR_IF_ERR_EXIT(dnx_tsn_cqf_active_set_hw_set(unit, core_id, !set));

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_params_base_params_hw_set
                                (unit, core_id, set, gate_id, tsn_counter_base_time, TRUE));

                SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_base_time_sw_set(unit, core_id, egr_tsn_if, config_change_time));

                set = !set;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
