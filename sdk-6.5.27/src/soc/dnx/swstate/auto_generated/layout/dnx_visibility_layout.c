
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_visibility_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_visibility_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_visibility[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_VISIBILITY_INFO_DB_NOF_PARAMS)];

shr_error_e
dnx_visibility_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_VISIBILITY_INFO_DB);
    layout_array_dnx_visibility[idx].name = "visibility_info_db";
    layout_array_dnx_visibility[idx].type = "visibility_params";
    layout_array_dnx_visibility[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_visibility[idx].size_of = sizeof(visibility_params);
    layout_array_dnx_visibility[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_visibility[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_VISIBILITY_INFO_DB__FIRST);
    layout_array_dnx_visibility[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_VISIBILITY_INFO_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_VISIBILITY_INFO_DB__VISIB_MODE);
    layout_array_dnx_visibility[idx].name = "visibility_info_db__visib_mode";
    layout_array_dnx_visibility[idx].type = "int";
    layout_array_dnx_visibility[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_visibility[idx].size_of = sizeof(int);
    layout_array_dnx_visibility[idx].parent  = 0;
    layout_array_dnx_visibility[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_visibility[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_VISIBILITY_INFO_DB, layout_array_dnx_visibility, sw_state_layout_array[unit][DNX_VISIBILITY_MODULE_ID], DNX_SW_STATE_VISIBILITY_INFO_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
