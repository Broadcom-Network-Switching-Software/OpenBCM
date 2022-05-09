
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/srv6_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/srv6_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/srv6_layout.h>





int
srv6_modes_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_SRV6_MODES,
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
        DNX_SW_STATE_SRV6_MODES,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_SRV6_MODES,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        srv6_modes_struct,
        DNX_SW_STATE_SRV6_MODES_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "srv6_modes_init",
        DNX_SW_STATE_DIAG_ALLOC,
        srv6_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_SRV6_MODES,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]),
        "srv6_modes[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES,
        srv6_modes_info,
        SRV6_MODES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID]),
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_egress_is_psp_set(int unit, uint8 egress_is_psp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp,
        egress_is_psp,
        uint8,
        0,
        "srv6_modes_egress_is_psp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        &egress_is_psp,
        "srv6_modes[%d]->egress_is_psp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        srv6_modes_info,
        SRV6_MODES_EGRESS_IS_PSP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_egress_is_psp_get(int unit, uint8 *egress_is_psp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        egress_is_psp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        DNX_SW_STATE_DIAG_READ);

    *egress_is_psp = ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->egress_is_psp,
        "srv6_modes[%d]->egress_is_psp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
        srv6_modes_info,
        SRV6_MODES_EGRESS_IS_PSP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_encap_is_reduced_set(int unit, uint8 encap_is_reduced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced,
        encap_is_reduced,
        uint8,
        0,
        "srv6_modes_encap_is_reduced_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        &encap_is_reduced,
        "srv6_modes[%d]->encap_is_reduced",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        srv6_modes_info,
        SRV6_MODES_ENCAP_IS_REDUCED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_encap_is_reduced_get(int unit, uint8 *encap_is_reduced)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        encap_is_reduced);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        DNX_SW_STATE_DIAG_READ);

    *encap_is_reduced = ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->encap_is_reduced,
        "srv6_modes[%d]->encap_is_reduced",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
        srv6_modes_info,
        SRV6_MODES_ENCAP_IS_REDUCED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_gsid_prefix_is_64b_set(int unit, uint8 gsid_prefix_is_64b)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->gsid_prefix_is_64b,
        gsid_prefix_is_64b,
        uint8,
        0,
        "srv6_modes_gsid_prefix_is_64b_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        &gsid_prefix_is_64b,
        "srv6_modes[%d]->gsid_prefix_is_64b",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        srv6_modes_info,
        SRV6_MODES_GSID_PREFIX_IS_64B_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
srv6_modes_gsid_prefix_is_64b_get(int unit, uint8 *gsid_prefix_is_64b)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        sw_state_roots_array[unit][SRV6_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        gsid_prefix_is_64b);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        DNX_SW_STATE_DIAG_READ);

    *gsid_prefix_is_64b = ((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->gsid_prefix_is_64b;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        &((srv6_modes_struct*)sw_state_roots_array[unit][SRV6_MODULE_ID])->gsid_prefix_is_64b,
        "srv6_modes[%d]->gsid_prefix_is_64b",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
        srv6_modes_info,
        SRV6_MODES_GSID_PREFIX_IS_64B_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][SRV6_MODULE_ID]);

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
	,
		{
		
		srv6_modes_gsid_prefix_is_64b_set,
		srv6_modes_gsid_prefix_is_64b_get}
	}
;
#undef BSL_LOG_MODULE
