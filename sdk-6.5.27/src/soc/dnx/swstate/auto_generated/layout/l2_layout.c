
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/l2_types.h>
#include <soc/dnx/swstate/auto_generated/layout/l2_layout.h>

dnxc_sw_state_layout_t layout_array_l2[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT_NOF_PARAMS)];

shr_error_e
l2_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT);
    layout_array_l2[idx].name = "l2_db_context";
    layout_array_l2[idx].type = "l2_db_t";
    layout_array_l2[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].size_of = sizeof(l2_db_t);
    layout_array_l2[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT__FIRST);
    layout_array_l2[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT__FID_PROFILES_CHANGED);
    layout_array_l2[idx].name = "l2_db_context__fid_profiles_changed";
    layout_array_l2[idx].type = "uint8";
    layout_array_l2[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].size_of = sizeof(uint8);
    layout_array_l2[idx].parent  = 0;
    layout_array_l2[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_L2_DB_CONTEXT__ROUTING_LEARNING);
    layout_array_l2[idx].name = "l2_db_context__routing_learning";
    layout_array_l2[idx].type = "uint32";
    layout_array_l2[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].size_of = sizeof(uint32);
    layout_array_l2[idx].parent  = 0;
    layout_array_l2[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_l2[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_L2_DB_CONTEXT, layout_array_l2, sw_state_layout_array[unit][L2_MODULE_ID], DNX_SW_STATE_L2_DB_CONTEXT_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
