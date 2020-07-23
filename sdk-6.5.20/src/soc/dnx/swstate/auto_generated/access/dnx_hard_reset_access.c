
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
#include <soc/dnx/swstate/auto_generated/access/dnx_hard_reset_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_hard_reset_diagnostic.h>



dnx_hard_reset_db_t* dnx_hard_reset_db_dummy = NULL;



int
dnx_hard_reset_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_HARD_RESET_MODULE_ID,
        ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]),
        "dnx_hard_reset_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_hard_reset_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_hard_reset_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_HARD_RESET_MODULE_ID,
        ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]),
        "dnx_hard_reset_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        dnx_hard_reset_db_info,
        DNX_HARD_RESET_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_hard_reset_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_callback_set(int unit, bcm_switch_hard_reset_callback_t callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_HARD_RESET_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.callback,
        callback,
        bcm_switch_hard_reset_callback_t,
        0,
        "dnx_hard_reset_db_hard_reset_callback_callback_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_HARD_RESET_MODULE_ID,
        &callback,
        "dnx_hard_reset_db[%d]->hard_reset_callback.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        dnx_hard_reset_db_info,
        DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_callback_get(int unit, bcm_switch_hard_reset_callback_t *callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        callback);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_HARD_RESET_MODULE_ID);

    *callback = ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.callback;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_HARD_RESET_MODULE_ID,
        &((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.callback,
        "dnx_hard_reset_db[%d]->hard_reset_callback.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        dnx_hard_reset_db_info,
        DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_userdata_set(int unit, dnx_hard_reset_callback_userdata_t userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_HARD_RESET_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.userdata,
        userdata,
        dnx_hard_reset_callback_userdata_t,
        0,
        "dnx_hard_reset_db_hard_reset_callback_userdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_HARD_RESET_MODULE_ID,
        &userdata,
        "dnx_hard_reset_db[%d]->hard_reset_callback.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        dnx_hard_reset_db_info,
        DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_USERDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_hard_reset_db_hard_reset_callback_userdata_get(int unit, dnx_hard_reset_callback_userdata_t *userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        userdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_HARD_RESET_MODULE_ID);

    *userdata = ((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.userdata;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_HARD_RESET_MODULE_ID,
        &((dnx_hard_reset_db_t*)sw_state_roots_array[unit][DNX_HARD_RESET_MODULE_ID])->hard_reset_callback.userdata,
        "dnx_hard_reset_db[%d]->hard_reset_callback.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_HARD_RESET_MODULE_ID,
        dnx_hard_reset_db_info,
        DNX_HARD_RESET_DB_HARD_RESET_CALLBACK_USERDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_hard_reset_db_cbs dnx_hard_reset_db = 	{
	
	dnx_hard_reset_db_is_init,
	dnx_hard_reset_db_init,
		{
		
			{
			
			dnx_hard_reset_db_hard_reset_callback_callback_set,
			dnx_hard_reset_db_hard_reset_callback_callback_get}
		,
			{
			
			dnx_hard_reset_db_hard_reset_callback_userdata_set,
			dnx_hard_reset_db_hard_reset_callback_userdata_get}
		}
	}
;
#undef BSL_LOG_MODULE
