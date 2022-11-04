
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/interrupt_types.h>
#include <soc/dnx/swstate/auto_generated/layout/interrupt_layout.h>

dnxc_sw_state_layout_t layout_array_interrupt[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB_NOF_PARAMS)];

shr_error_e
interrupt_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB);
    layout_array_interrupt[idx].name = "intr_db";
    layout_array_interrupt[idx].type = "soc_dnx_intr_t";
    layout_array_interrupt[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].size_of = sizeof(soc_dnx_intr_t);
    layout_array_interrupt[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__FIRST);
    layout_array_interrupt[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__FLAGS);
    layout_array_interrupt[idx].name = "intr_db__flags";
    layout_array_interrupt[idx].type = "uint32";
    layout_array_interrupt[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].size_of = sizeof(uint32);
    layout_array_interrupt[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_interrupt[idx].parent  = 0;
    layout_array_interrupt[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT);
    layout_array_interrupt[idx].name = "intr_db__storm_timed_count";
    layout_array_interrupt[idx].type = "uint32";
    layout_array_interrupt[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].size_of = sizeof(uint32);
    layout_array_interrupt[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_interrupt[idx].parent  = 0;
    layout_array_interrupt[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD);
    layout_array_interrupt[idx].name = "intr_db__storm_timed_period";
    layout_array_interrupt[idx].type = "uint32";
    layout_array_interrupt[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].size_of = sizeof(uint32);
    layout_array_interrupt[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_interrupt[idx].parent  = 0;
    layout_array_interrupt[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INTR_DB__STORM_NOMINAL);
    layout_array_interrupt[idx].name = "intr_db__storm_nominal";
    layout_array_interrupt[idx].type = "uint32";
    layout_array_interrupt[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].size_of = sizeof(uint32);
    layout_array_interrupt[idx].parent  = 0;
    layout_array_interrupt[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_interrupt[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_INTR_DB, layout_array_interrupt, sw_state_layout_array[unit][INTERRUPT_MODULE_ID], DNX_SW_STATE_INTR_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
