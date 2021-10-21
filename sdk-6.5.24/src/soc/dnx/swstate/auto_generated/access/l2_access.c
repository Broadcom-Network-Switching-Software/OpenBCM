
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/l2_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/l2_diagnostic.h>





int
l2_db_context_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]),
        "l2_db_context[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        L2_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        l2_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "l2_db_context_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID]),
        "l2_db_context[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(l2_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_set(int unit, uint8 fid_profiles_changed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed,
        fid_profiles_changed,
        uint8,
        0,
        "l2_db_context_fid_profiles_changed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &fid_profiles_changed,
        "l2_db_context[%d]->fid_profiles_changed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_FID_PROFILES_CHANGED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_fid_profiles_changed_get(int unit, uint8 *fid_profiles_changed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        fid_profiles_changed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *fid_profiles_changed = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->fid_profiles_changed,
        "l2_db_context[%d]->fid_profiles_changed",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_FID_PROFILES_CHANGED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_set(int unit, uint32 routing_learning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        L2_MODULE_ID,
        ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning,
        routing_learning,
        uint32,
        0,
        "l2_db_context_routing_learning_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        L2_MODULE_ID,
        &routing_learning,
        "l2_db_context[%d]->routing_learning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_ROUTING_LEARNING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
l2_db_context_routing_learning_get(int unit, uint32 *routing_learning)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        L2_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        L2_MODULE_ID,
        sw_state_roots_array[unit][L2_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        L2_MODULE_ID,
        routing_learning);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        L2_MODULE_ID);

    *routing_learning = ((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        L2_MODULE_ID,
        &((l2_db_t*)sw_state_roots_array[unit][L2_MODULE_ID])->routing_learning,
        "l2_db_context[%d]->routing_learning",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        L2_MODULE_ID,
        l2_db_context_info,
        L2_DB_CONTEXT_ROUTING_LEARNING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





l2_db_context_cbs l2_db_context = 	{
	
	l2_db_context_is_init,
	l2_db_context_init,
		{
		
		l2_db_context_fid_profiles_changed_set,
		l2_db_context_fid_profiles_changed_get}
	,
		{
		
		l2_db_context_routing_learning_set,
		l2_db_context_routing_learning_get}
	}
;
#undef BSL_LOG_MODULE
