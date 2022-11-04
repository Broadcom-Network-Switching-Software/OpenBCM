
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/rx_trap_types.h>
#include <soc/dnx/swstate/auto_generated/layout/rx_trap_layout.h>

dnxc_sw_state_layout_t layout_array_rx_trap[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO_NOF_PARAMS)];

shr_error_e
rx_trap_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO);
    layout_array_rx_trap[idx].name = "rx_trap_info";
    layout_array_rx_trap[idx].type = "rx_trap_info_t";
    layout_array_rx_trap[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_rx_trap[idx].size_of = sizeof(rx_trap_info_t);
    layout_array_rx_trap[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_rx_trap[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__FIRST);
    layout_array_rx_trap[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO);
    layout_array_rx_trap[idx].name = "rx_trap_info__user_trap_info";
    layout_array_rx_trap[idx].type = "rx_trap_user_info_t";
    layout_array_rx_trap[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_rx_trap[idx].size_of = sizeof(rx_trap_user_info_t);
    layout_array_rx_trap[idx].array_indexes[0].num_elements = dnx_data_trap.ingress.nof_user_defined_traps_get(unit);
    layout_array_rx_trap[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_rx_trap[idx].parent  = 0;
    layout_array_rx_trap[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__FIRST);
    layout_array_rx_trap[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__LAST)-1;
    layout_array_rx_trap[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_LEAF_STRUCT;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_RX_TRAP_INFO__USER_TRAP_INFO__NAME);
    layout_array_rx_trap[idx].name = "rx_trap_info__user_trap_info__name";
    layout_array_rx_trap[idx].type = "char";
    layout_array_rx_trap[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_rx_trap[idx].size_of = sizeof(char);
    layout_array_rx_trap[idx].array_indexes[0].num_elements = BCM_RX_TRAP_MAX_NAME_LEN;
    layout_array_rx_trap[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_rx_trap[idx].parent  = 1;
    layout_array_rx_trap[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_rx_trap[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_RX_TRAP_INFO, layout_array_rx_trap, sw_state_layout_array[unit][RX_TRAP_MODULE_ID], DNX_SW_STATE_RX_TRAP_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
