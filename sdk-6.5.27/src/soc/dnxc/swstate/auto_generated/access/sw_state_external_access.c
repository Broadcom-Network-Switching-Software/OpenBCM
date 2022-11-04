
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/access/sw_state_external_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_external.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/sw_state_external_diagnostic.h>
#include <soc/dnxc/swstate/auto_generated/layout/sw_state_external_layout.h>





int
sw_state_external_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]),
        "sw_state_external[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sw_state_external_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        sw_state_external_t,
        DNX_SW_STATE_SW_STATE_EXTERNAL_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "sw_state_external_init",
        DNX_SW_STATE_DIAG_ALLOC,
        sw_state_external_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]),
        "sw_state_external[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL,
        sw_state_external_info,
        SW_STATE_EXTERNAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]),
        sw_state_layout_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sw_state_external_offsets_set(int unit, uint32 offsets_idx_0, uint32 offsets_idx_1, uint32 offsets)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        offsets_idx_0,
        DNXC_SW_STATE_EXTERNAL_COUNT);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        offsets_idx_1,
        DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[offsets_idx_0][offsets_idx_1],
        offsets,
        uint32,
        0,
        "sw_state_external_offsets_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        &offsets,
        "sw_state_external[%d]->offsets[%d][%d]",
        unit, offsets_idx_0, offsets_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        sw_state_external_info,
        SW_STATE_EXTERNAL_OFFSETS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
sw_state_external_offsets_get(int unit, uint32 offsets_idx_0, uint32 offsets_idx_1, uint32 *offsets)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        offsets_idx_0,
        DNXC_SW_STATE_EXTERNAL_COUNT);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        offsets);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        offsets_idx_1,
        DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        DNX_SW_STATE_DIAG_READ);

    *offsets = ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[offsets_idx_0][offsets_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        &((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[offsets_idx_0][offsets_idx_1],
        "sw_state_external[%d]->offsets[%d][%d]",
        unit, offsets_idx_0, offsets_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
        sw_state_external_info,
        SW_STATE_EXTERNAL_OFFSETS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




sw_state_external_cbs sw_state_external = 	{
	
	sw_state_external_is_init,
	sw_state_external_init,
		{
		
		sw_state_external_offsets_set,
		sw_state_external_offsets_get}
	}
;
#undef BSL_LOG_MODULE
