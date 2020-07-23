
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
#include <soc/dnx/swstate/auto_generated/access/srv6_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/srv6_diagnostic.h>



srv6_modes_struct* srv6_modes_dummy = NULL;



int
srv6_modes_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        SRV6_MODULE_ID,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]),
        "srv6_modes[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        SRV6_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        srv6_modes_struct,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "srv6_modes_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        SRV6_MODULE_ID,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]),
        "srv6_modes[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        SRV6_MODULE_ID,
        srv6_modes_info,
        SRV6_MODES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(srv6_modes_struct),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_egress_is_psp_set(int unit, uint8 egress_is_psp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SRV6_MODULE_ID,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SRV6_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SRV6_MODULE_ID,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp,
        egress_is_psp,
        uint8,
        0,
        "srv6_modes_egress_is_psp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SRV6_MODULE_ID,
        &egress_is_psp,
        "srv6_modes[%d]->egress_is_psp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SRV6_MODULE_ID,
        srv6_modes_info,
        SRV6_MODES_EGRESS_IS_PSP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_egress_is_psp_get(int unit, uint8 *egress_is_psp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SRV6_MODULE_ID,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SRV6_MODULE_ID,
        egress_is_psp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SRV6_MODULE_ID);

    *egress_is_psp = ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SRV6_MODULE_ID,
        &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp,
        "srv6_modes[%d]->egress_is_psp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SRV6_MODULE_ID,
        srv6_modes_info,
        SRV6_MODES_EGRESS_IS_PSP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_encap_is_reduced_set(int unit, uint8 encap_is_reduced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SRV6_MODULE_ID,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SRV6_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SRV6_MODULE_ID,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced,
        encap_is_reduced,
        uint8,
        0,
        "srv6_modes_encap_is_reduced_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SRV6_MODULE_ID,
        &encap_is_reduced,
        "srv6_modes[%d]->encap_is_reduced",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SRV6_MODULE_ID,
        srv6_modes_info,
        SRV6_MODES_ENCAP_IS_REDUCED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
srv6_modes_encap_is_reduced_get(int unit, uint8 *encap_is_reduced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SRV6_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SRV6_MODULE_ID,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SRV6_MODULE_ID,
        encap_is_reduced);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SRV6_MODULE_ID);

    *encap_is_reduced = ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SRV6_MODULE_ID,
        &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced,
        "srv6_modes[%d]->encap_is_reduced",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SRV6_MODULE_ID,
        srv6_modes_info,
        SRV6_MODES_ENCAP_IS_REDUCED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





srv6_modes_cbs srv6_modes = 	{
	
	srv6_modes_is_init,
	srv6_modes_init,
		{
		
		srv6_modes_egress_is_psp_set,
		srv6_modes_egress_is_psp_get}
	,
		{
		
		srv6_modes_encap_is_reduced_set,
		srv6_modes_encap_is_reduced_get}
	}
;
#undef BSL_LOG_MODULE
