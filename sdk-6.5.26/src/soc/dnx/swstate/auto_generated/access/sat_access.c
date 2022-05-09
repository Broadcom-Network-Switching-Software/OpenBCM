
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/sat_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/sat_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/sat_layout.h>





int
sat_pkt_header_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]),
        "sat_pkt_header_info[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sat_pkt_header_info_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sat_pkt_header_info_t,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sat_pkt_header_info_init",
        DNX_SW_STATE_DIAG_ALLOC,
        sat_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]),
        "sat_pkt_header_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]),
        sw_state_layout_array[unit][SAT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sat_pkt_header_info_pkt_header_base_set(int unit, uint32 pkt_header_base_idx_0, uint16 pkt_header_base)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        pkt_header_base_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0],
        pkt_header_base,
        uint16,
        0,
        "sat_pkt_header_info_pkt_header_base_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        &pkt_header_base,
        "sat_pkt_header_info[%d]->pkt_header_base[%d]",
        unit, pkt_header_base_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][SAT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sat_pkt_header_info_pkt_header_base_get(int unit, uint32 pkt_header_base_idx_0, uint16 *pkt_header_base)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        pkt_header_base_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        pkt_header_base);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        DNX_SW_STATE_DIAG_READ);

    *pkt_header_base = ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        &((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0],
        "sat_pkt_header_info[%d]->pkt_header_base[%d]",
        unit, pkt_header_base_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][SAT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sat_pkt_header_info_pkt_header_base_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        uint16,
        dnx_data_sat.generator.nof_gtf_ids_get(unit)+1,
        sw_state_layout_array[unit][SAT_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sat_pkt_header_info_pkt_header_base_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        "sat_pkt_header_info[%d]->pkt_header_base",
        unit,
        dnx_data_sat.generator.nof_gtf_ids_get(unit)+1 * sizeof(uint16) / sizeof(*((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][SAT_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sat_pkt_header_info_pkt_header_base_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        is_allocated);

    DNX_SW_STATE_IS_ALLOC(
        unit,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        is_allocated,
        DNXC_SW_STATE_NONE,
        "sat_pkt_header_info_pkt_header_base_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ISALLOC,
        DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        "sat_pkt_header_info[%d]->pkt_header_base",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




sat_pkt_header_info_cbs sat_pkt_header_info = 	{
	
	sat_pkt_header_info_is_init,
	sat_pkt_header_info_init,
		{
		
		sat_pkt_header_info_pkt_header_base_set,
		sat_pkt_header_info_pkt_header_base_get,
		sat_pkt_header_info_pkt_header_base_alloc,
		sat_pkt_header_info_pkt_header_base_is_allocated}
	}
;
#undef BSL_LOG_MODULE
