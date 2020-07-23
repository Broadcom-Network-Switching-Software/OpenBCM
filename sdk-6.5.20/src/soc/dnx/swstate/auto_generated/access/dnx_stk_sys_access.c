
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_stk_sys_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_stk_sys_diagnostic.h>



dnx_stk_sys_db_t* dnx_stk_sys_db_dummy = NULL;



int
dnx_stk_sys_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]),
        "dnx_stk_sys_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_STK_SYS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_stk_sys_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_stk_sys_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]),
        "dnx_stk_sys_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_stk_sys_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_nof_fap_ids_set(int unit, int nof_fap_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids,
        nof_fap_ids,
        int,
        0,
        "dnx_stk_sys_db_nof_fap_ids_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_STK_SYS_MODULE_ID,
        &nof_fap_ids,
        "dnx_stk_sys_db[%d]->nof_fap_ids",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_NOF_FAP_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_nof_fap_ids_get(int unit, int *nof_fap_ids)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_STK_SYS_MODULE_ID,
        nof_fap_ids);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    *nof_fap_ids = ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_STK_SYS_MODULE_ID,
        &((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids,
        "dnx_stk_sys_db[%d]->nof_fap_ids",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_NOF_FAP_IDS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_nof_fap_ids_inc(int unit, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids,
        inc_value,
        int,
        0,
        "dnx_stk_sys_db_nof_fap_ids_inc");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_NOF_FAP_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_nof_fap_ids_dec(int unit, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->nof_fap_ids,
        dec_value,
        int,
        0,
        "dnx_stk_sys_db_nof_fap_ids_dec");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_NOF_FAP_IDS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_module_enable_done_set(int unit, int module_enable_done)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_STK_SYS_MODULE_ID,
        ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->module_enable_done,
        module_enable_done,
        int,
        0,
        "dnx_stk_sys_db_module_enable_done_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_STK_SYS_MODULE_ID,
        &module_enable_done,
        "dnx_stk_sys_db[%d]->module_enable_done",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_MODULE_ENABLE_DONE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stk_sys_db_module_enable_done_get(int unit, int *module_enable_done)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_STK_SYS_MODULE_ID,
        sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_STK_SYS_MODULE_ID,
        module_enable_done);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_STK_SYS_MODULE_ID);

    *module_enable_done = ((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->module_enable_done;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_STK_SYS_MODULE_ID,
        &((dnx_stk_sys_db_t*)sw_state_roots_array[unit][DNX_STK_SYS_MODULE_ID])->module_enable_done,
        "dnx_stk_sys_db[%d]->module_enable_done",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_STK_SYS_MODULE_ID,
        dnx_stk_sys_db_info,
        DNX_STK_SYS_DB_MODULE_ENABLE_DONE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_stk_sys_db_cbs dnx_stk_sys_db = 	{
	
	dnx_stk_sys_db_is_init,
	dnx_stk_sys_db_init,
		{
		
		dnx_stk_sys_db_nof_fap_ids_set,
		dnx_stk_sys_db_nof_fap_ids_get,
		dnx_stk_sys_db_nof_fap_ids_inc,
		dnx_stk_sys_db_nof_fap_ids_dec}
	,
		{
		
		dnx_stk_sys_db_module_enable_done_set,
		dnx_stk_sys_db_module_enable_done_get}
	}
;
#endif  
#undef BSL_LOG_MODULE
