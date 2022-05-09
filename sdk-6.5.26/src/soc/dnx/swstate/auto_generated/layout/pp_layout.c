
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/pp_types.h>
#include <soc/dnx/swstate/auto_generated/layout/pp_layout.h>

dnxc_sw_state_layout_t layout_array_pp[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_PP_DB_NOF_PARAMS)];

shr_error_e
pp_init_layout_structure(int unit)
{

    uint8 pp_db__pp_stage_kbr_select__default_val = 255;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_PP_DB);
    layout_array_pp[idx].name = "pp_db";
    layout_array_pp[idx].type = "dnx_pp_sw_state_t";
    layout_array_pp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_pp[idx].size_of = sizeof(dnx_pp_sw_state_t);
    layout_array_pp[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_pp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_PP_DB__FIRST);
    layout_array_pp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_PP_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_PP_DB__PP_STAGE_KBR_SELECT);
    layout_array_pp[idx].name = "pp_db__pp_stage_kbr_select";
    layout_array_pp[idx].type = "uint8";
    layout_array_pp[idx].doc = "the selector of pp stages (VTT23, VTT45, FWD12)";
    layout_array_pp[idx].default_value= &(pp_db__pp_stage_kbr_select__default_val);
    layout_array_pp[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_pp[idx].size_of = sizeof(uint8);
    layout_array_pp[idx].array_indexes[0].num_elements = MAX_NOF_PP_KBR;
    layout_array_pp[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_pp[idx].array_indexes[1].num_elements = MAX_NOF_KBR_SELCTORS;
    layout_array_pp[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_pp[idx].parent  = 0;
    layout_array_pp[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_pp[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_PP_DB, layout_array_pp, sw_state_layout_array[unit][PP_MODULE_ID], DNX_SW_STATE_PP_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
