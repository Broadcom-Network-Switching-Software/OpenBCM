
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_epfc_sw_state_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_epfc_sw_state_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_epfc_sw_state_layout.h>





int
dnx_epfc_interface_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]),
        "dnx_epfc_interface_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_epfc_sw_state_t,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_epfc_interface_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_epfc_sw_state_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]),
        "dnx_epfc_interface_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB,
        dnx_epfc_interface_db_info,
        DNX_EPFC_INTERFACE_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]),
        sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_is_initialized_set(int unit, uint32 is_initialized)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->is_initialized,
        is_initialized,
        uint32,
        0,
        "dnx_epfc_interface_db_is_initialized_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        &is_initialized,
        "dnx_epfc_interface_db[%d]->is_initialized",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        dnx_epfc_interface_db_info,
        DNX_EPFC_INTERFACE_DB_IS_INITIALIZED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_is_initialized_get(int unit, uint32 *is_initialized)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        is_initialized);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        DNX_SW_STATE_DIAG_READ);

    *is_initialized = ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->is_initialized;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        &((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->is_initialized,
        "dnx_epfc_interface_db[%d]->is_initialized",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED,
        dnx_epfc_interface_db_info,
        DNX_EPFC_INTERFACE_DB_IS_INITIALIZED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_uc_num_set(int unit, uint32 uc_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->uc_num,
        uc_num,
        uint32,
        0,
        "dnx_epfc_interface_db_uc_num_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        &uc_num,
        "dnx_epfc_interface_db[%d]->uc_num",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        dnx_epfc_interface_db_info,
        DNX_EPFC_INTERFACE_DB_UC_NUM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_epfc_interface_db_uc_num_get(int unit, uint32 *uc_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        uc_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        DNX_SW_STATE_DIAG_READ);

    *uc_num = ((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->uc_num;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        &((dnx_epfc_sw_state_t*)sw_state_roots_array[unit][DNX_EPFC_SW_STATE_MODULE_ID])->uc_num,
        "dnx_epfc_interface_db[%d]->uc_num",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM,
        dnx_epfc_interface_db_info,
        DNX_EPFC_INTERFACE_DB_UC_NUM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_epfc_interface_db_cbs dnx_epfc_interface_db = 	{
	
	dnx_epfc_interface_db_is_init,
	dnx_epfc_interface_db_init,
		{
		
		dnx_epfc_interface_db_is_initialized_set,
		dnx_epfc_interface_db_is_initialized_get}
	,
		{
		
		dnx_epfc_interface_db_uc_num_set,
		dnx_epfc_interface_db_uc_num_get}
	}
;
#undef BSL_LOG_MODULE
