
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
#include <soc/dnx/swstate/auto_generated/access/flush_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/flush_diagnostic.h>



flush_db_t* flush_db_context_dummy = NULL;



int
flush_db_context_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        FLUSH_MODULE_ID,
        ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID]),
        "flush_db_context[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        FLUSH_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        flush_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "flush_db_context_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        FLUSH_MODULE_ID,
        ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID]),
        "flush_db_context[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        FLUSH_MODULE_ID,
        flush_db_context_info,
        FLUSH_DB_CONTEXT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(flush_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_nof_valid_rules_set(int unit, uint32 nof_valid_rules)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLUSH_MODULE_ID,
        sw_state_roots_array[unit][FLUSH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLUSH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        FLUSH_MODULE_ID,
        ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->nof_valid_rules,
        nof_valid_rules,
        uint32,
        0,
        "flush_db_context_nof_valid_rules_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        FLUSH_MODULE_ID,
        &nof_valid_rules,
        "flush_db_context[%d]->nof_valid_rules",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLUSH_MODULE_ID,
        flush_db_context_info,
        FLUSH_DB_CONTEXT_NOF_VALID_RULES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_nof_valid_rules_get(int unit, uint32 *nof_valid_rules)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLUSH_MODULE_ID,
        sw_state_roots_array[unit][FLUSH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLUSH_MODULE_ID,
        nof_valid_rules);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLUSH_MODULE_ID);

    *nof_valid_rules = ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->nof_valid_rules;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        FLUSH_MODULE_ID,
        &((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->nof_valid_rules,
        "flush_db_context[%d]->nof_valid_rules",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLUSH_MODULE_ID,
        flush_db_context_info,
        FLUSH_DB_CONTEXT_NOF_VALID_RULES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_flush_in_bulk_set(int unit, uint8 flush_in_bulk)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLUSH_MODULE_ID,
        sw_state_roots_array[unit][FLUSH_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLUSH_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        FLUSH_MODULE_ID,
        ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->flush_in_bulk,
        flush_in_bulk,
        uint8,
        0,
        "flush_db_context_flush_in_bulk_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        FLUSH_MODULE_ID,
        &flush_in_bulk,
        "flush_db_context[%d]->flush_in_bulk",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLUSH_MODULE_ID,
        flush_db_context_info,
        FLUSH_DB_CONTEXT_FLUSH_IN_BULK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
flush_db_context_flush_in_bulk_get(int unit, uint8 *flush_in_bulk)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        FLUSH_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        FLUSH_MODULE_ID,
        sw_state_roots_array[unit][FLUSH_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        FLUSH_MODULE_ID,
        flush_in_bulk);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        FLUSH_MODULE_ID);

    *flush_in_bulk = ((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->flush_in_bulk;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        FLUSH_MODULE_ID,
        &((flush_db_t*)sw_state_roots_array[unit][FLUSH_MODULE_ID])->flush_in_bulk,
        "flush_db_context[%d]->flush_in_bulk",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        FLUSH_MODULE_ID,
        flush_db_context_info,
        FLUSH_DB_CONTEXT_FLUSH_IN_BULK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





flush_db_context_cbs flush_db_context = 	{
	
	flush_db_context_is_init,
	flush_db_context_init,
		{
		
		flush_db_context_nof_valid_rules_set,
		flush_db_context_nof_valid_rules_get}
	,
		{
		
		flush_db_context_flush_in_bulk_set,
		flush_db_context_flush_in_bulk_get}
	}
;
#undef BSL_LOG_MODULE
