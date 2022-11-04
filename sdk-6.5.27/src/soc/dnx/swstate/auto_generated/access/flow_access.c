
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/flow_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/flow_layout.h>





int
flow_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_FLOW_DB,
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
        DNX_SW_STATE_FLOW_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_FLOW_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        flow_sw_state_t,
        DNX_SW_STATE_FLOW_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "flow_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        flow_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_FLOW_DB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]),
        "flow_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB,
        flow_db_info,
        FLOW_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID]),
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_flow_application_info_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        dnx_flow_app_common_config_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][FLOW_MODULE_ID],
        0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "flow_db_flow_application_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        "flow_db[%d]->flow_application_info",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_flow_app_common_config_t) / sizeof(*((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_flow_application_info_is_verify_disabled_set(int unit, uint32 flow_application_info_idx_0, uint8 is_verify_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled,
        is_verify_disabled,
        uint8,
        0,
        "flow_db_flow_application_info_is_verify_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        &is_verify_disabled,
        "flow_db[%d]->flow_application_info[%d].is_verify_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_flow_application_info_is_verify_disabled_get(int unit, uint32 flow_application_info_idx_0, uint8 *is_verify_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        is_verify_disabled);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        DNX_SW_STATE_DIAG_READ);

    *is_verify_disabled = ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_verify_disabled,
        "flow_db[%d]->flow_application_info[%d].is_verify_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_VERIFY_DISABLED,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_VERIFY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_flow_application_info_is_error_recovery_disabled_set(int unit, uint32 flow_application_info_idx_0, uint8 is_error_recovery_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled,
        is_error_recovery_disabled,
        uint8,
        0,
        "flow_db_flow_application_info_is_error_recovery_disabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        &is_error_recovery_disabled,
        "flow_db[%d]->flow_application_info[%d].is_error_recovery_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_flow_application_info_is_error_recovery_disabled_get(int unit, uint32 flow_application_info_idx_0, uint8 *is_error_recovery_disabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info,
        flow_application_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        is_error_recovery_disabled);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        DNX_SW_STATE_DIAG_READ);

    *is_error_recovery_disabled = ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->flow_application_info[flow_application_info_idx_0].is_error_recovery_disabled,
        "flow_db[%d]->flow_application_info[%d].is_error_recovery_disabled",
        unit, flow_application_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_FLOW_DB__FLOW_APPLICATION_INFO__IS_ERROR_RECOVERY_DISABLED,
        flow_db_info,
        FLOW_DB_FLOW_APPLICATION_INFO_IS_ERROR_RECOVERY_DISABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        init_info,
        dbal_tables_e,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_find(int unit, const dbal_tables_e *key, uint32 *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_insert(int unit, const dbal_tables_e *key, const uint32 *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const dbal_tables_e *key, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_valid_phases_per_dbal_table_delete(int unit, const dbal_tables_e *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->valid_phases_per_dbal_table,
        "flow_db[%d]->valid_phases_per_dbal_table",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__VALID_PHASES_PER_DBAL_TABLE,
        flow_db_info,
        FLOW_DB_VALID_PHASES_PER_DBAL_TABLE_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        init_info,
        flow_app_encap_info_t,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_find(int unit, const flow_app_encap_info_t *key, uint32 *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_insert(int unit, const flow_app_encap_info_t *key, const uint32 *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const flow_app_encap_info_t *key, const uint32 *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_dbal_table_to_valid_result_types_delete(int unit, const flow_app_encap_info_t *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->dbal_table_to_valid_result_types,
        "flow_db[%d]->dbal_table_to_valid_result_types",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__DBAL_TABLE_TO_VALID_RESULT_TYPES,
        flow_db_info,
        FLOW_DB_DBAL_TABLE_TO_VALID_RESULT_TYPES_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_create(int unit, sw_state_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB_CREATE);

    SW_STATE_HTB_CREATE(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        init_info,
        int,
        ingress_gport_sw_info_t,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCREATE,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_find(int unit, const int *key, ingress_gport_sw_info_t *value, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_FIND(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        key,
        value,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_HTBFIND,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_insert(int unit, const int *key, const ingress_gport_sw_info_t *value, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_INSERT(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        key,
        value,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBINSERT,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_get_next(int unit, SW_STATE_HASH_TABLE_ITER *iter, const int *key, const ingress_gport_sw_info_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_GET_NEXT(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        iter,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_HTBGET_NEXT,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_CLEAR(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_HTBCLEAR,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
flow_db_ingress_gport_to_match_info_htb_delete(int unit, const int *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        sw_state_roots_array[unit][FLOW_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        DNX_SW_STATE_DIAG_HTB);

    SW_STATE_HTB_DELETE(
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        ((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_HTBDELETE,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        &((flow_sw_state_t*)sw_state_roots_array[unit][FLOW_MODULE_ID])->ingress_gport_to_match_info_htb,
        "flow_db[%d]->ingress_gport_to_match_info_htb",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_FLOW_DB__INGRESS_GPORT_TO_MATCH_INFO_HTB,
        flow_db_info,
        FLOW_DB_INGRESS_GPORT_TO_MATCH_INFO_HTB_INFO,
        DNX_SW_STATE_DIAG_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][FLOW_MODULE_ID]);

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
	,
		{
		
		flow_db_ingress_gport_to_match_info_htb_create,
		flow_db_ingress_gport_to_match_info_htb_find,
		flow_db_ingress_gport_to_match_info_htb_insert,
		flow_db_ingress_gport_to_match_info_htb_get_next,
		flow_db_ingress_gport_to_match_info_htb_clear,
		flow_db_ingress_gport_to_match_info_htb_delete}
	}
;
#undef BSL_LOG_MODULE
