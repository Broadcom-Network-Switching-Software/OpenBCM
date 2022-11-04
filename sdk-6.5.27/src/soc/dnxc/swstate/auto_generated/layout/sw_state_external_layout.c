
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/types/sw_state_external_types.h>
#include <soc/dnxc/swstate/auto_generated/layout/sw_state_external_layout.h>

dnxc_sw_state_layout_t layout_array_sw_state_external[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_EXTERNAL_NOF_PARAMS)];

shr_error_e
sw_state_external_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_EXTERNAL);
    layout_array_sw_state_external[idx].name = "sw_state_external";
    layout_array_sw_state_external[idx].type = "sw_state_external_t";
    layout_array_sw_state_external[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sw_state_external[idx].size_of = sizeof(sw_state_external_t);
    layout_array_sw_state_external[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sw_state_external[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_EXTERNAL__FIRST);
    layout_array_sw_state_external[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_EXTERNAL__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS);
    layout_array_sw_state_external[idx].name = "sw_state_external__offsets";
    layout_array_sw_state_external[idx].type = "uint32";
    layout_array_sw_state_external[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sw_state_external[idx].size_of = sizeof(uint32);
    layout_array_sw_state_external[idx].array_indexes[0].num_elements = DNXC_SW_STATE_EXTERNAL_COUNT;
    layout_array_sw_state_external[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_sw_state_external[idx].array_indexes[1].num_elements = DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE;
    layout_array_sw_state_external[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_sw_state_external[idx].parent  = 0;
    layout_array_sw_state_external[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sw_state_external[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SW_STATE_EXTERNAL, layout_array_sw_state_external, sw_state_layout_array[unit][SW_STATE_EXTERNAL_MODULE_ID], DNX_SW_STATE_SW_STATE_EXTERNAL_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
