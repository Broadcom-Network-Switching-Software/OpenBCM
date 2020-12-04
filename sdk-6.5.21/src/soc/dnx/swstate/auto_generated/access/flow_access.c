
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/flow_diagnostic.h>



flow_sw_state_t* flow_db_dummy = NULL;



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
	}
;
#undef BSL_LOG_MODULE
