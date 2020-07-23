
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/instru_counters_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/instru_counters_diagnostic.h>



instru_sync_counters_sw_state_t* instru_sync_counters_dummy = NULL;



int
instru_sync_counters_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]),
        "instru_sync_counters[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        instru_sync_counters_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "instru_sync_counters_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]),
        "instru_sync_counters[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(instru_sync_counters_sw_state_t),
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.sync_counters_started,
        FALSE);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.sync_counters_started,
        FALSE);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_sync_counters_started_set(int unit, uint8 sync_counters_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.sync_counters_started,
        sync_counters_started,
        uint8,
        0,
        "instru_sync_counters_icgm_countes_sync_counters_started_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &sync_counters_started,
        "instru_sync_counters[%d]->icgm_countes.sync_counters_started",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_SYNC_COUNTERS_STARTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_sync_counters_started_get(int unit, uint8 *sync_counters_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sync_counters_started);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *sync_counters_started = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.sync_counters_started;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.sync_counters_started,
        "instru_sync_counters[%d]->icgm_countes.sync_counters_started",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_SYNC_COUNTERS_STARTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_tod_1588_timestamp_start_set(int unit, uint64 tod_1588_timestamp_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.tod_1588_timestamp_start,
        tod_1588_timestamp_start,
        uint64,
        0,
        "instru_sync_counters_icgm_countes_tod_1588_timestamp_start_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &tod_1588_timestamp_start,
        "instru_sync_counters[%d]->icgm_countes.tod_1588_timestamp_start",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_tod_1588_timestamp_start_get(int unit, uint64 *tod_1588_timestamp_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        tod_1588_timestamp_start);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *tod_1588_timestamp_start = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.tod_1588_timestamp_start;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.tod_1588_timestamp_start,
        "instru_sync_counters[%d]->icgm_countes.tod_1588_timestamp_start",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_per_core_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info,
        sync_counters_icgm_per_core_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "instru_sync_counters_icgm_countes_per_core_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info,
        "instru_sync_counters[%d]->icgm_countes.per_core_info",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sync_counters_icgm_per_core_t) / sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_PER_CORE_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_set(int unit, uint32 per_core_info_idx_0, uint32 record_last_read_nof)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info,
        per_core_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info[per_core_info_idx_0].record_last_read_nof,
        record_last_read_nof,
        uint32,
        0,
        "instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &record_last_read_nof,
        "instru_sync_counters[%d]->icgm_countes.per_core_info[%d].record_last_read_nof",
        unit, per_core_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_PER_CORE_INFO_RECORD_LAST_READ_NOF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_get(int unit, uint32 per_core_info_idx_0, uint32 *record_last_read_nof)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info,
        per_core_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        record_last_read_nof);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *record_last_read_nof = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info[per_core_info_idx_0].record_last_read_nof;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info[per_core_info_idx_0].record_last_read_nof,
        "instru_sync_counters[%d]->icgm_countes.per_core_info[%d].record_last_read_nof",
        unit, per_core_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_ICGM_COUNTES_PER_CORE_INFO_RECORD_LAST_READ_NOF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_sync_counters_started_set(int unit, uint8 sync_counters_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.sync_counters_started,
        sync_counters_started,
        uint8,
        0,
        "instru_sync_counters_nif_countes_sync_counters_started_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &sync_counters_started,
        "instru_sync_counters[%d]->nif_countes.sync_counters_started",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_SYNC_COUNTERS_STARTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_sync_counters_started_get(int unit, uint8 *sync_counters_started)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sync_counters_started);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *sync_counters_started = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.sync_counters_started;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.sync_counters_started,
        "instru_sync_counters[%d]->nif_countes.sync_counters_started",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_SYNC_COUNTERS_STARTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_tod_1588_timestamp_start_set(int unit, uint64 tod_1588_timestamp_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.tod_1588_timestamp_start,
        tod_1588_timestamp_start,
        uint64,
        0,
        "instru_sync_counters_nif_countes_tod_1588_timestamp_start_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &tod_1588_timestamp_start,
        "instru_sync_counters[%d]->nif_countes.tod_1588_timestamp_start",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_tod_1588_timestamp_start_get(int unit, uint64 *tod_1588_timestamp_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        tod_1588_timestamp_start);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *tod_1588_timestamp_start = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.tod_1588_timestamp_start;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.tod_1588_timestamp_start,
        "instru_sync_counters[%d]->nif_countes.tod_1588_timestamp_start",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_TOD_1588_TIMESTAMP_START_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_per_ethu_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info,
        sync_counters_nif_per_eth_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "instru_sync_counters_nif_countes_per_ethu_info_alloc");

    for(uint32 per_ethu_info_idx_0 = 0;
         per_ethu_info_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         per_ethu_info_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0],
        sync_counters_nif_per_eth_info_t,
        dnx_data_nif.eth.ethu_nof_per_core_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "instru_sync_counters_nif_countes_per_ethu_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info,
        "instru_sync_counters[%d]->nif_countes.per_ethu_info",
        unit,
        dnx_data_nif.eth.ethu_nof_per_core_get(unit) * sizeof(sync_counters_nif_per_eth_info_t) / sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info)+(dnx_data_nif.eth.ethu_nof_per_core_get(unit) * sizeof(sync_counters_nif_per_eth_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_set(int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1, uint32 record_last_read_nof_idx_0, uint32 record_last_read_nof)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info,
        per_ethu_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0],
        per_ethu_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof,
        record_last_read_nof_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof[record_last_read_nof_idx_0],
        record_last_read_nof,
        uint32,
        0,
        "instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        INSTRU_COUNTERS_MODULE_ID,
        &record_last_read_nof,
        "instru_sync_counters[%d]->nif_countes.per_ethu_info[%d][%d].record_last_read_nof[%d]",
        unit, per_ethu_info_idx_0, per_ethu_info_idx_1, record_last_read_nof_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_RECORD_LAST_READ_NOF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_get(int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1, uint32 record_last_read_nof_idx_0, uint32 *record_last_read_nof)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info,
        per_ethu_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        record_last_read_nof);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0],
        per_ethu_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof,
        record_last_read_nof_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    *record_last_read_nof = ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof[record_last_read_nof_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        INSTRU_COUNTERS_MODULE_ID,
        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof[record_last_read_nof_idx_0],
        "instru_sync_counters[%d]->nif_countes.per_ethu_info[%d][%d].record_last_read_nof[%d]",
        unit, per_ethu_info_idx_0, per_ethu_info_idx_1, record_last_read_nof_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_RECORD_LAST_READ_NOF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_alloc(int unit, uint32 per_ethu_info_idx_0, uint32 per_ethu_info_idx_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info,
        per_ethu_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0],
        per_ethu_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        INSTRU_COUNTERS_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof,
        uint32,
        dnx_data_nif.eth.max_nof_lanes_in_ethu_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        INSTRU_COUNTERS_MODULE_ID,
        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof,
        "instru_sync_counters[%d]->nif_countes.per_ethu_info[%d][%d].record_last_read_nof",
        unit, per_ethu_info_idx_0, per_ethu_info_idx_1,
        dnx_data_nif.eth.max_nof_lanes_in_ethu_get(unit) * sizeof(uint32) / sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[per_ethu_info_idx_0][per_ethu_info_idx_1].record_last_read_nof));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        INSTRU_COUNTERS_MODULE_ID,
        instru_sync_counters_info,
        INSTRU_SYNC_COUNTERS_NIF_COUNTES_PER_ETHU_INFO_RECORD_LAST_READ_NOF_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





instru_sync_counters_cbs instru_sync_counters = 	{
	
	instru_sync_counters_is_init,
	instru_sync_counters_init,
		{
		
			{
			
			instru_sync_counters_icgm_countes_sync_counters_started_set,
			instru_sync_counters_icgm_countes_sync_counters_started_get}
		,
			{
			
			instru_sync_counters_icgm_countes_tod_1588_timestamp_start_set,
			instru_sync_counters_icgm_countes_tod_1588_timestamp_start_get}
		,
			{
			
			instru_sync_counters_icgm_countes_per_core_info_alloc,
				{
				
				instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_set,
				instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_get}
			}
		}
	,
		{
		
			{
			
			instru_sync_counters_nif_countes_sync_counters_started_set,
			instru_sync_counters_nif_countes_sync_counters_started_get}
		,
			{
			
			instru_sync_counters_nif_countes_tod_1588_timestamp_start_set,
			instru_sync_counters_nif_countes_tod_1588_timestamp_start_get}
		,
			{
			
			instru_sync_counters_nif_countes_per_ethu_info_alloc,
				{
				
				instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_set,
				instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_get,
				instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_alloc}
			}
		}
	}
;
#undef BSL_LOG_MODULE
