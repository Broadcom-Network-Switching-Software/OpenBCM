
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/sat_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/sat_diagnostic.h>





int
sat_pkt_header_info_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SAT_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        SAT_MODULE_ID,
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
        SAT_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        SAT_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sat_pkt_header_info_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sat_pkt_header_info_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]),
        "sat_pkt_header_info[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SAT_MODULE_ID,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]));

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sat_pkt_header_info_pkt_header_base_set(int unit, uint32 pkt_header_base_idx_0, uint16 pkt_header_base)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SAT_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SAT_MODULE_ID,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        pkt_header_base_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SAT_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0],
        pkt_header_base,
        uint16,
        0,
        "sat_pkt_header_info_pkt_header_base_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        SAT_MODULE_ID,
        &pkt_header_base,
        "sat_pkt_header_info[%d]->pkt_header_base[%d]",
        unit, pkt_header_base_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SAT_MODULE_ID,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sat_pkt_header_info_pkt_header_base_get(int unit, uint32 pkt_header_base_idx_0, uint16 *pkt_header_base)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SAT_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SAT_MODULE_ID,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        pkt_header_base_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SAT_MODULE_ID,
        pkt_header_base);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SAT_MODULE_ID);

    *pkt_header_base = ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        SAT_MODULE_ID,
        &((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[pkt_header_base_idx_0],
        "sat_pkt_header_info[%d]->pkt_header_base[%d]",
        unit, pkt_header_base_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SAT_MODULE_ID,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sat_pkt_header_info_pkt_header_base_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SAT_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SAT_MODULE_ID,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        SAT_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        uint16,
        dnx_data_sat.generator.nof_gtf_ids_get(unit)+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sat_pkt_header_info_pkt_header_base_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        SAT_MODULE_ID,
        ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base,
        "sat_pkt_header_info[%d]->pkt_header_base",
        unit,
        dnx_data_sat.generator.nof_gtf_ids_get(unit)+1 * sizeof(uint16) / sizeof(*((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        SAT_MODULE_ID,
        sat_pkt_header_info_info,
        SAT_PKT_HEADER_INFO_PKT_HEADER_BASE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
sat_pkt_header_info_pkt_header_base_is_allocated(int unit, uint8 *is_allocated)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        SAT_MODULE_ID,
        SW_STATE_FUNC_IS_ALLOCATED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        SAT_MODULE_ID,
        sw_state_roots_array[unit][SAT_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        SAT_MODULE_ID,
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
        SAT_MODULE_ID,
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
