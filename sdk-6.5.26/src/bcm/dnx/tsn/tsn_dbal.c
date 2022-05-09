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
dnx_tsn_counter_ptp_time_hw_get(
    int unit,
    bcm_ptp_timestamp_t * ptp_time)
{
    uint32 seconds;
    uint32 nanoseconds;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TSN_PTP_TIME_VALUE, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NANOSECS, INST_SINGLE, &nanoseconds);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SECS, INST_SINGLE, &seconds);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Set output struct */
    ptp_time->nanoseconds = nanoseconds;
    COMPILER_64_SET(ptp_time->seconds, 0, seconds);

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
 * ------------------------------------------------------------------------------------------
 * --                 TAS (Time Aware Scheduling)
 * ------------------------------------------------------------------------------------------
 */

/*
 * ------------------------------------------------------------------------------------------
 * --                 CQF (Cyclic Queuing and Forwarding)
 * ------------------------------------------------------------------------------------------
 */
