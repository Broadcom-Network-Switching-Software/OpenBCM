
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/flow_diagnostic.h>





int
flow_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]),
        "flow_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        FLOW_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        flow_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "flow_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]),
        "flow_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(flow_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        dnx_flow_app_common_config_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "flow_db_flow_application_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        "flow_db[%d]->flow_application_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_flow_app_common_config_t) / sizeof(*((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_verify_disabled_set(int unit, uint32 flow_application_info_idx_0, uint8 is_verify_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled,
        is_verify_disabled,
        uint8,
        0,
        "flow_db_flow_application_info_is_verify_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        FLOW_MODULE_ID,
        &is_verify_disabled,
        "flow_db[%d]->flow_application_info[%d].is_verify_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_verify_disabled_get(int unit, uint32 flow_application_info_idx_0, uint8 *is_verify_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        is_verify_disabled);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    *is_verify_disabled = ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled,
        "flow_db[%d]->flow_application_info[%d].is_verify_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_error_recovery_disabled_set(int unit, uint32 flow_application_info_idx_0, uint8 is_error_recovery_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled,
        is_error_recovery_disabled,
        uint8,
        0,
        "flow_db_flow_application_info_is_error_recovery_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        FLOW_MODULE_ID,
        &is_error_recovery_disabled,
        "flow_db[%d]->flow_application_info[%d].is_error_recovery_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_flow_application_info_is_error_recovery_disabled_get(int unit, uint32 flow_application_info_idx_0, uint8 *is_error_recovery_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        is_error_recovery_disabled);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    *is_error_recovery_disabled = ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled,
        "flow_db[%d]->flow_application_info[%d].is_error_recovery_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_CREATE(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        init_info,
        dbal_tables_e,
        uint32,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_find(int unit, const dbal_tables_e *key, uint32 *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_FIND(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_insert(int unit, const dbal_tables_e *key, const uint32 *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_INSERT(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const dbal_tables_e *key, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_valid_phases_per_dbal_table_delete(int unit, const dbal_tables_e *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_DELETE(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_CREATE(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        init_info,
        flow_app_encap_info_t,
        uint32,
        FALSE,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_find(int unit, const flow_app_encap_info_t *key, uint32 *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_FIND(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_insert(int unit, const flow_app_encap_info_t *key, const uint32 *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_INSERT(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const flow_app_encap_info_t *key, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_GET_NEXT(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_CLEAR(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flow_db_dbal_table_to_valid_result_types_delete(int unit, const flow_app_encap_info_t *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLOW_MODULE_ID,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLOW_MODULE_ID,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLOW_MODULE_ID);

    SW_STATE_HTB_DELETE(
        FLOW_MODULE_ID,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        FLOW_MODULE_ID,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        FLOW_MODULE_ID,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}





flow_db_cbs flow_db = 	{
	
	flow_db_is_init,
	flow_db_init,
		{
		
		flow_db_flow_application_info_alloc,
			{
			
			flow_db_flow_application_info_is_verify_disabled_set,
			flow_db_flow_application_info_is_verify_disabled_get}
		,
			{
			
			flow_db_flow_application_info_is_error_recovery_disabled_set,
			flow_db_flow_application_info_is_error_recovery_disabled_get}
		}
	,
		{
		
		flow_db_valid_phases_per_dbal_table_create,
		flow_db_valid_phases_per_dbal_table_find,
		flow_db_valid_phases_per_dbal_table_insert,
		flow_db_valid_phases_per_dbal_table_get_next,
		flow_db_valid_phases_per_dbal_table_clear,
		flow_db_valid_phases_per_dbal_table_delete}
	,
		{
		
		flow_db_dbal_table_to_valid_result_types_create,
		flow_db_dbal_table_to_valid_result_types_find,
		flow_db_dbal_table_to_valid_result_types_insert,
		flow_db_dbal_table_to_valid_result_types_get_next,
		flow_db_dbal_table_to_valid_result_types_clear,
		flow_db_dbal_table_to_valid_result_types_delete}
	}
;
#undef BSL_LOG_MODULE
