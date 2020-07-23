
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
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_synce_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_synce_diagnostic.h>



dnx_algo_synce_db_t* dnx_algo_synce_db_dummy = NULL;



int
dnx_algo_synce_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_ALGO_SYNCE_MODULE_ID,
        ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]),
        "dnx_algo_synce_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_synce_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_synce_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_ALGO_SYNCE_MODULE_ID,
        ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]),
        "dnx_algo_synce_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        dnx_algo_synce_db_info,
        DNX_ALGO_SYNCE_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_algo_synce_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set(int unit, int master_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.master_synce_enabled,
        master_synce_enabled,
        int,
        0,
        "dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_SYNCE_MODULE_ID,
        &master_synce_enabled,
        "dnx_algo_synce_db[%d]->fabric_synce_status.master_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        dnx_algo_synce_db_info,
        DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_MASTER_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get(int unit, int *master_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        master_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID);

    *master_synce_enabled = ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.master_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_SYNCE_MODULE_ID,
        &((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.master_synce_enabled,
        "dnx_algo_synce_db[%d]->fabric_synce_status.master_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        dnx_algo_synce_db_info,
        DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_MASTER_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set(int unit, int slave_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.slave_synce_enabled,
        slave_synce_enabled,
        int,
        0,
        "dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_SYNCE_MODULE_ID,
        &slave_synce_enabled,
        "dnx_algo_synce_db[%d]->fabric_synce_status.slave_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        dnx_algo_synce_db_info,
        DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_SLAVE_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get(int unit, int *slave_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        slave_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID);

    *slave_synce_enabled = ((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.slave_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_SYNCE_MODULE_ID,
        &((dnx_algo_synce_db_t*)sw_state_roots_array[unit][DNX_ALGO_SYNCE_MODULE_ID])->fabric_synce_status.slave_synce_enabled,
        "dnx_algo_synce_db[%d]->fabric_synce_status.slave_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_SYNCE_MODULE_ID,
        dnx_algo_synce_db_info,
        DNX_ALGO_SYNCE_DB_FABRIC_SYNCE_STATUS_SLAVE_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_algo_synce_db_cbs dnx_algo_synce_db = 	{
	
	dnx_algo_synce_db_is_init,
	dnx_algo_synce_db_init,
		{
		
			{
			
			dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set,
			dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get}
		,
			{
			
			dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set,
			dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get}
		}
	}
;
#undef BSL_LOG_MODULE
