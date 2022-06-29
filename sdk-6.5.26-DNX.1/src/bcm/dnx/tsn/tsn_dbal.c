/**
 * \file src/bcm/dnx/tsn/tsn_dbal.c
 * 
 *
 * DBAL access functions for TSN
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_dbal.h"
/*
 * }
 */

/*
 * ------------------------------------------------------------------------------------------
 * --                 TSN Counter
 * ------------------------------------------------------------------------------------------
 */

/*
 * see .h file
 */
shr_error_e
dnx_tsn_counter_enable_hw_set(
    int unit,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_COUNTER_CONTROL, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_tsn_counter_tas_tod_enable_hw_set(
    int unit,
    uint32 ts_source,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_TOD_CONTROL, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TS_SOURCE, INST_SINGLE, ts_source);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_tsn_counter_tick_period_hw_set(
    int unit,
    uint32 tick_period_ns,
    uint32 tick_period_min_ns)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_COUNTER_CONTROL, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TICK_PERIOD_NS, INST_SINGLE, tick_period_ns);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TICK_PERIOD_MIN_NS, INST_SINGLE, tick_period_min_ns);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_tsn_counter_start_attributes_hw_set(
    int unit,
    uint64 start_tas_tod_time,
    uint64 start_tsn_value,
    uint32 start_trigger)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_COUNTER_START_ATTRIBUTES, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_START_TAS_TOD, INST_SINGLE, start_tas_tod_time);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_START_TSN_COUNTER, INST_SINGLE, start_tsn_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TRIGGER, INST_SINGLE, start_trigger);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_start_trigger_down_hw_poll(
    int unit,
    sal_usecs_t timeout,
    int32 min_polls)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_COUNTER_START_ATTRIBUTES, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, timeout, min_polls, entry_handle_id, DBAL_FIELD_START_TRIGGER, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_curr_time_hw_get(
    int unit,
    uint64 *curr_time)
{
    uint32 seconds;
    uint32 nanoseconds;
    uint32 entry_handle_id;
    uint64 calc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_PTP_TIME_VALUE, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NANOSECS, INST_SINGLE, &nanoseconds);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SECS, INST_SINGLE, &seconds);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Set output */
    COMPILER_64_SET(calc, 0, seconds);
    COMPILER_64_UMUL_32(calc, 1000000000);
    COMPILER_64_ADD_32(calc, nanoseconds);
    COMPILER_64_COPY(*curr_time, calc);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_counter_synced_counters_value_hw_get(
    int unit,
    uint64 *tsn_counter,
    uint64 *tas_tod)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_SYNCED_COUNTERS, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_TSN_COUNTER, INST_SINGLE, tsn_counter);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_TSN_TAS_TOD, INST_SINGLE, tas_tod);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 TAF (Time Aware Filtering)
 * ------------------------------------------------------------------------------------------
 */

/*
 * See .h file
 */
shr_error_e
dnx_tsn_taf_global_config_hw_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAF_GLOBAL_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_INIT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_SET_0_START, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_SET_0_END, INST_SINGLE,
                                 dnx_data_tsn.taf.nof_gates_get(unit) - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_SET_1_START, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_SET_1_END, INST_SINGLE,
                                 dnx_data_tsn.taf.nof_gates_get(unit) - 1);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_taf_active_set_hw_get(
    int unit,
    uint32 *active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAF_GLOBAL_CONFIG, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TAF_ACTIVE_SET, INST_SINGLE, active_set);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_taf_active_set_hw_set(
    int unit,
    uint32 active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAF_GLOBAL_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_ACTIVE_SET, INST_SINGLE, active_set);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_taf_gate_params_hw_set(
    int unit,
    uint32 set,
    uint32 taf_gate,
    uint32 last_ti_entry,
    uint32 is_active,
    uint64 base_time,
    uint32 first_gate_state,
    uint32 ti_index_start)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAF_GATE_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TAF_GATE, taf_gate);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_TI_ENTRY, INST_SINGLE, last_ti_entry);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ACTIVE, INST_SINGLE, is_active);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_BASE_TIME, INST_SINGLE, base_time);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_GATE_STATE, INST_SINGLE, first_gate_state);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX_START, INST_SINGLE, ti_index_start);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_taf_gate_control_list_hw_set(
    int unit,
    uint32 set,
    uint32 ti_index,
    uint32 num_tick_period,
    uint32 next_gate_state)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAF_GATE_CONTROL_LIST, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX, ti_index);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_TICK_PERIODS, INST_SINGLE, num_tick_period);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_GATE_STATE, INST_SINGLE, next_gate_state);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 TAS (Time Aware Scheduling)
 * ------------------------------------------------------------------------------------------
 */

/**
 * \brief - Init TAS offset values.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] tas_port -  Tas port
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_tsn_tas_offset_init(
    int unit,
    int tas_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SET, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_PORT, tas_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_START, INST_SINGLE,
                                 tas_port * dnx_data_tsn.tas.max_time_intervals_entries_get(unit));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_tas_hw_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_INIT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_ENABLE, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set tas port offsets*/
    for (int tas_port = 0; tas_port < dnx_data_tsn.tas.nof_tas_ports_get(unit); tas_port++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_tas_offset_init(unit, tas_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_tas_active_set_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 *active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TAS_ACTIVE_SET, INST_SINGLE, active_set);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_tas_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_ACTIVE_SET, INST_SINGLE, active_set);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_tas_port_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 nof_ti,
    uint32 is_active,
    uint64 base_time,
    uint32 first_gate_state,
    uint32 first_stop_preemptable,
    uint32 queue_base,
    dbal_enum_value_field_egq_ps_mode_e ps_mode,
    uint32 esb_port_num)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_PORT, tas_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_TI_ENTRIES, INST_SINGLE, nof_ti);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ACTIVE, INST_SINGLE, is_active);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_BASE_TIME, INST_SINGLE, base_time);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_GATE_STATE, INST_SINGLE, first_gate_state);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_STOP_PREEMPTABLE, INST_SINGLE,
                                 first_stop_preemptable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_BASE, INST_SINGLE, queue_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_PS_MODE, INST_SINGLE, ps_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_PORT_NUM, INST_SINGLE, esb_port_num);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_tas_port_control_list_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 tas_port,
    uint32 ti_index,
    uint64 nof_tick_periods,
    uint32 next_gate_state,
    uint32 next_stop_preemptable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_TAS_PORT_CONTROL_LIST, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TAS_PORT, tas_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX, ti_index);

    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_NUM_TICK_PERIODS, INST_SINGLE, nof_tick_periods);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_GATE_STATE, INST_SINGLE, next_gate_state);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_STOP_PREEMPTABLE, INST_SINGLE,
                                 next_stop_preemptable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * ------------------------------------------------------------------------------------------
 * --                 CQF (Cyclic Queuing and Forwarding)
 * ------------------------------------------------------------------------------------------
 */

/** see header file **/
shr_error_e
dnx_tsn_cqf_active_set_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 *active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CQF_ACTIVE_SET, INST_SINGLE, active_set);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_active_set_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 active_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_ACTIVE_SET, INST_SINGLE, active_set);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_port_params_active_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 is_active)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ACTIVE, INST_SINGLE, is_active);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_port_params_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint64 base_time,
    uint32 queue_base,
    dbal_enum_value_field_egq_ps_mode_e ps_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);

    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_BASE_TIME, INST_SINGLE, base_time);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_BASE, INST_SINGLE, queue_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_PS_MODE, INST_SINGLE, ps_mode);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_port_params_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ACTIVE, INST_SINGLE, 0);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_BASE_TIME, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_BASE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_PS_MODE, INST_SINGLE, 0);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 nof_tick_periods)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_CONTROL_LIST, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX, ti_index);

    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_NUM_TICK_PERIODS, INST_SINGLE, nof_tick_periods);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_tsn_cqf_port_control_list_nof_tick_periods_hw_get(
    int unit,
    bcm_core_t core_id,
    uint32 set,
    uint32 cqf_port,
    uint32 ti_index,
    uint64 *nof_tick_periods)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_CONTROL_LIST, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET, set);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX, ti_index);

    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_NUM_TICK_PERIODS, INST_SINGLE, nof_tick_periods);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_cqf_port_control_list_gate_and_phase_hw_init(
    int unit,
    bcm_core_t core_id,
    uint32 cqf_port,
    uint32 ti_index,
    uint32 next_gate_state,
    uint32 next_phase)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_CONTROL_LIST, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SET, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_INDEX, ti_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_GATE_STATE, INST_SINGLE, next_gate_state);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PHASE, INST_SINGLE, next_phase);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_tsn_cqf_global_config_hw_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_INIT, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_tsn_cqf_global_config_hw_enable(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_GLOBAL_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_ENABLE, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_tsn_cqf_port_params_hw_init(
    int unit,
    int cqf_port)
{
    uint32 entry_handle_id;
    uint32 first_phase = dnx_data_tsn.cqf.port_control_list_init_get(unit, 0)->phase;
    uint32 first_gate_state = dnx_data_tsn.cqf.port_control_list_init_get(unit, 0)->gate_state;
    uint32 nof_time_intervals = dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_CQF_PORT_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SET, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CQF_PORT, cqf_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_TI_ENTRIES, INST_SINGLE, nof_time_intervals);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_GATE_STATE, INST_SINGLE, first_gate_state);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_PHASE, INST_SINGLE, first_phase);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TI_START, INST_SINGLE,
                                 cqf_port * dnx_data_tsn.cqf.max_time_intervals_entries_get(unit));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
