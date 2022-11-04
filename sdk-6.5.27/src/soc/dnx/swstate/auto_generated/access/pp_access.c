
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/pp_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/pp_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/pp_layout.h>





int
pp_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_PP_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_PP_DB,
        ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID]),
        "pp_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
pp_db_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_PP_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_PP_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_pp_sw_state_t,
        DNX_SW_STATE_PP_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "pp_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        pp_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_PP_DB,
        ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID]),
        "pp_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_PP_DB,
        pp_db_info,
        PP_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID]),
        sw_state_layout_array[unit][PP_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], MAX_NOF_PP_KBR)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], MAX_NOF_KBR_SELCTORS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[def_val_idx[1]][def_val_idx[2]],
        255);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
pp_db_pp_stage_kbr_select_set(int unit, uint32 pp_stage_kbr_select_idx_0, uint32 pp_stage_kbr_select_idx_1, uint8 pp_stage_kbr_select)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        sw_state_roots_array[unit][PP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pp_stage_kbr_select_idx_0,
        MAX_NOF_PP_KBR);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pp_stage_kbr_select_idx_1,
        MAX_NOF_KBR_SELCTORS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[pp_stage_kbr_select_idx_0][pp_stage_kbr_select_idx_1],
        pp_stage_kbr_select,
        uint8,
        0,
        "pp_db_pp_stage_kbr_select_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        &pp_stage_kbr_select,
        "pp_db[%d]->pp_stage_kbr_select[%d][%d]",
        unit, pp_stage_kbr_select_idx_0, pp_stage_kbr_select_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        pp_db_info,
        PP_DB_PP_STAGE_KBR_SELECT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][PP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
pp_db_pp_stage_kbr_select_get(int unit, uint32 pp_stage_kbr_select_idx_0, uint32 pp_stage_kbr_select_idx_1, uint8 *pp_stage_kbr_select)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        sw_state_roots_array[unit][PP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pp_stage_kbr_select_idx_0,
        MAX_NOF_PP_KBR);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        pp_stage_kbr_select);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        pp_stage_kbr_select_idx_1,
        MAX_NOF_KBR_SELCTORS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        DNX_SW_STATE_DIAG_READ);

    *pp_stage_kbr_select = ((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[pp_stage_kbr_select_idx_0][pp_stage_kbr_select_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        &((dnx_pp_sw_state_t*)sw_state_roots_array[unit][PP_MODULE_ID])->pp_stage_kbr_select[pp_stage_kbr_select_idx_0][pp_stage_kbr_select_idx_1],
        "pp_db[%d]->pp_stage_kbr_select[%d][%d]",
        unit, pp_stage_kbr_select_idx_0, pp_stage_kbr_select_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT,
        pp_db_info,
        PP_DB_PP_STAGE_KBR_SELECT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][PP_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




pp_db_cbs pp_db = 	{
	
	pp_db_is_init,
	pp_db_init,
		{
		
		pp_db_pp_stage_kbr_select_set,
		pp_db_pp_stage_kbr_select_get}
	}
;
#undef BSL_LOG_MODULE
