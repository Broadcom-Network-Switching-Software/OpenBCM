
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
#include <soc/dnx/swstate/auto_generated/access/dnx_traps_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_traps_diagnostic.h>



dnx_rx_traps_info_t* dnx_rx_traps_info_dummy = NULL;



int
dnx_rx_traps_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TRAPS_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_TRAPS_MODULE_ID,
        ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]),
        "dnx_rx_traps_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_traps_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TRAPS_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_TRAPS_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_rx_traps_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_rx_traps_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_TRAPS_MODULE_ID,
        ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]),
        "dnx_rx_traps_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_TRAPS_MODULE_ID,
        dnx_rx_traps_info_info,
        DNX_RX_TRAPS_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_rx_traps_info_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_traps_info_etpp_keep_fabric_headers_profile_set(int unit, uint8 etpp_keep_fabric_headers_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TRAPS_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TRAPS_MODULE_ID,
        sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TRAPS_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_TRAPS_MODULE_ID,
        ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID])->etpp_keep_fabric_headers_profile,
        etpp_keep_fabric_headers_profile,
        uint8,
        0,
        "dnx_rx_traps_info_etpp_keep_fabric_headers_profile_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_TRAPS_MODULE_ID,
        &etpp_keep_fabric_headers_profile,
        "dnx_rx_traps_info[%d]->etpp_keep_fabric_headers_profile",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TRAPS_MODULE_ID,
        dnx_rx_traps_info_info,
        DNX_RX_TRAPS_INFO_ETPP_KEEP_FABRIC_HEADERS_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_traps_info_etpp_keep_fabric_headers_profile_get(int unit, uint8 *etpp_keep_fabric_headers_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_TRAPS_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_TRAPS_MODULE_ID,
        sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_TRAPS_MODULE_ID,
        etpp_keep_fabric_headers_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_TRAPS_MODULE_ID);

    *etpp_keep_fabric_headers_profile = ((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID])->etpp_keep_fabric_headers_profile;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_TRAPS_MODULE_ID,
        &((dnx_rx_traps_info_t*)sw_state_roots_array[unit][DNX_TRAPS_MODULE_ID])->etpp_keep_fabric_headers_profile,
        "dnx_rx_traps_info[%d]->etpp_keep_fabric_headers_profile",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_TRAPS_MODULE_ID,
        dnx_rx_traps_info_info,
        DNX_RX_TRAPS_INFO_ETPP_KEEP_FABRIC_HEADERS_PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_rx_traps_info_cbs dnx_rx_traps_info = 	{
	
	dnx_rx_traps_info_is_init,
	dnx_rx_traps_info_init,
		{
		
		dnx_rx_traps_info_etpp_keep_fabric_headers_profile_set,
		dnx_rx_traps_info_etpp_keep_fabric_headers_profile_get}
	}
;
#undef BSL_LOG_MODULE
