
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
#include <soc/dnx/swstate/auto_generated/access/dnx_time_sw_state_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_time_sw_state_diagnostic.h>



dnx_time_interface_sw_state_t* dnx_time_interface_db_dummy = NULL;



int
dnx_time_interface_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]),
        "dnx_time_interface_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_time_interface_sw_state_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_time_interface_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]),
        "dnx_time_interface_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_time_interface_sw_state_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_flags_set(int unit, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->flags,
        flags,
        uint32,
        0,
        "dnx_time_interface_db_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &flags,
        "dnx_time_interface_db[%d]->flags",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_flags_get(int unit, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *flags = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->flags,
        "dnx_time_interface_db[%d]->flags",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_id_set(int unit, int id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->id,
        id,
        int,
        0,
        "dnx_time_interface_db_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &id,
        "dnx_time_interface_db[%d]->id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_id_get(int unit, int *id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *id = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->id,
        "dnx_time_interface_db[%d]->id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_heartbeat_hz_set(int unit, int heartbeat_hz)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->heartbeat_hz,
        heartbeat_hz,
        int,
        0,
        "dnx_time_interface_db_heartbeat_hz_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &heartbeat_hz,
        "dnx_time_interface_db[%d]->heartbeat_hz",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_HEARTBEAT_HZ_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_heartbeat_hz_get(int unit, int *heartbeat_hz)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        heartbeat_hz);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *heartbeat_hz = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->heartbeat_hz;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->heartbeat_hz,
        "dnx_time_interface_db[%d]->heartbeat_hz",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_HEARTBEAT_HZ_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_clk_resolution_set(int unit, int clk_resolution)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->clk_resolution,
        clk_resolution,
        int,
        0,
        "dnx_time_interface_db_clk_resolution_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &clk_resolution,
        "dnx_time_interface_db[%d]->clk_resolution",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_CLK_RESOLUTION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_clk_resolution_get(int unit, int *clk_resolution)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        clk_resolution);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *clk_resolution = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->clk_resolution;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->clk_resolution,
        "dnx_time_interface_db[%d]->clk_resolution",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_CLK_RESOLUTION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_bitclock_hz_set(int unit, int bitclock_hz)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->bitclock_hz,
        bitclock_hz,
        int,
        0,
        "dnx_time_interface_db_bitclock_hz_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &bitclock_hz,
        "dnx_time_interface_db[%d]->bitclock_hz",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_BITCLOCK_HZ_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_bitclock_hz_get(int unit, int *bitclock_hz)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        bitclock_hz);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *bitclock_hz = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->bitclock_hz;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->bitclock_hz,
        "dnx_time_interface_db[%d]->bitclock_hz",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_BITCLOCK_HZ_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_status_set(int unit, int status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->status,
        status,
        int,
        0,
        "dnx_time_interface_db_status_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &status,
        "dnx_time_interface_db[%d]->status",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_STATUS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_time_interface_db_status_get(int unit, int *status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        status);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID);

    *status = ((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->status;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TIME_SW_STATE_MODULE_ID,
        &((dnx_time_interface_sw_state_t*)sw_state_roots_array[unit][DNX_TIME_SW_STATE_MODULE_ID])->status,
        "dnx_time_interface_db[%d]->status",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TIME_SW_STATE_MODULE_ID,
        dnx_time_interface_db_info,
        DNX_TIME_INTERFACE_DB_STATUS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_time_interface_db_cbs dnx_time_interface_db = 	{
	
	dnx_time_interface_db_is_init,
	dnx_time_interface_db_init,
		{
		
		dnx_time_interface_db_flags_set,
		dnx_time_interface_db_flags_get}
	,
		{
		
		dnx_time_interface_db_id_set,
		dnx_time_interface_db_id_get}
	,
		{
		
		dnx_time_interface_db_heartbeat_hz_set,
		dnx_time_interface_db_heartbeat_hz_get}
	,
		{
		
		dnx_time_interface_db_clk_resolution_set,
		dnx_time_interface_db_clk_resolution_get}
	,
		{
		
		dnx_time_interface_db_bitclock_hz_set,
		dnx_time_interface_db_bitclock_hz_get}
	,
		{
		
		dnx_time_interface_db_status_set,
		dnx_time_interface_db_status_get}
	}
;
#undef BSL_LOG_MODULE
