
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_epfc_sw_state_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_epfc_sw_state_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_epfc_sw_state[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB_NOF_PARAMS)];

shr_error_e
dnx_epfc_sw_state_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB);
    layout_array_dnx_epfc_sw_state[idx].name = "dnx_epfc_interface_db";
    layout_array_dnx_epfc_sw_state[idx].type = "dnx_epfc_sw_state_t";
    layout_array_dnx_epfc_sw_state[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].size_of = sizeof(dnx_epfc_sw_state_t);
    layout_array_dnx_epfc_sw_state[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__FIRST);
    layout_array_dnx_epfc_sw_state[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__IS_INITIALIZED);
    layout_array_dnx_epfc_sw_state[idx].name = "dnx_epfc_interface_db__is_initialized";
    layout_array_dnx_epfc_sw_state[idx].type = "uint32";
    layout_array_dnx_epfc_sw_state[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].size_of = sizeof(uint32);
    layout_array_dnx_epfc_sw_state[idx].parent  = 0;
    layout_array_dnx_epfc_sw_state[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_EPFC_INTERFACE_DB__UC_NUM);
    layout_array_dnx_epfc_sw_state[idx].name = "dnx_epfc_interface_db__uc_num";
    layout_array_dnx_epfc_sw_state[idx].type = "uint32";
    layout_array_dnx_epfc_sw_state[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].size_of = sizeof(uint32);
    layout_array_dnx_epfc_sw_state[idx].parent  = 0;
    layout_array_dnx_epfc_sw_state[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_epfc_sw_state[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_EPFC_INTERFACE_DB, layout_array_dnx_epfc_sw_state, sw_state_layout_array[unit][DNX_EPFC_SW_STATE_MODULE_ID], DNX_SW_STATE_DNX_EPFC_INTERFACE_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
