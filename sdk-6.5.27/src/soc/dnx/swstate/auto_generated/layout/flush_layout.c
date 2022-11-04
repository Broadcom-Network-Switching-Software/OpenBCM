
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/flush_types.h>
#include <soc/dnx/swstate/auto_generated/layout/flush_layout.h>

dnxc_sw_state_layout_t layout_array_flush[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT_NOF_PARAMS)];

shr_error_e
flush_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT);
    layout_array_flush[idx].name = "flush_db_context";
    layout_array_flush[idx].type = "flush_db_t";
    layout_array_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].size_of = sizeof(flush_db_t);
    layout_array_flush[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT__FIRST);
    layout_array_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT__NOF_VALID_RULES);
    layout_array_flush[idx].name = "flush_db_context__nof_valid_rules";
    layout_array_flush[idx].type = "uint32";
    layout_array_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].size_of = sizeof(uint32);
    layout_array_flush[idx].parent  = 0;
    layout_array_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_FLUSH_DB_CONTEXT__FLUSH_IN_BULK);
    layout_array_flush[idx].name = "flush_db_context__flush_in_bulk";
    layout_array_flush[idx].type = "uint8";
    layout_array_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].size_of = sizeof(uint8);
    layout_array_flush[idx].parent  = 0;
    layout_array_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_FLUSH_DB_CONTEXT, layout_array_flush, sw_state_layout_array[unit][FLUSH_MODULE_ID], DNX_SW_STATE_FLUSH_DB_CONTEXT_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
