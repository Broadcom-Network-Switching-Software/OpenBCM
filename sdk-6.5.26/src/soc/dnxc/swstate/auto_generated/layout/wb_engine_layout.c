
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/auto_generated/types/wb_engine_types.h>
#include <soc/dnxc/swstate/auto_generated/layout/wb_engine_layout.h>

dnxc_sw_state_layout_t layout_array_wb_engine[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE_NOF_PARAMS)];

shr_error_e
wb_engine_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE);
    layout_array_wb_engine[idx].name = "sw_state_wb_engine";
    layout_array_wb_engine[idx].type = "sw_state_wb_engine_t";
    layout_array_wb_engine[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].size_of = sizeof(sw_state_wb_engine_t);
    layout_array_wb_engine[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__FIRST);
    layout_array_wb_engine[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER);
    layout_array_wb_engine[idx].name = "sw_state_wb_engine__buffer";
    layout_array_wb_engine[idx].type = "sw_state_wb_engine_buffer_t*";
    layout_array_wb_engine[idx].doc = "wb engine buffer";
    layout_array_wb_engine[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].size_of = sizeof(sw_state_wb_engine_buffer_t*);
    layout_array_wb_engine[idx].array_indexes[0].name = "engine_id";
    layout_array_wb_engine[idx].array_indexes[0].num_elements = SOC_WB_ENGINE_NOF;
    layout_array_wb_engine[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_wb_engine[idx].array_indexes[1].name = "buffer_id";
    layout_array_wb_engine[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_wb_engine[idx].parent  = 0;
    layout_array_wb_engine[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__FIRST);
    layout_array_wb_engine[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__LAST);
    layout_array_wb_engine[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__INSTANCE);
    layout_array_wb_engine[idx].name = "sw_state_wb_engine__buffer__instance";
    layout_array_wb_engine[idx].type = "DNX_SW_STATE_BUFF*";
    layout_array_wb_engine[idx].doc = "wb engine buffer instance";
    layout_array_wb_engine[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].size_of = sizeof(DNX_SW_STATE_BUFF*);
    layout_array_wb_engine[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER;
    layout_array_wb_engine[idx].parent  = 1;
    layout_array_wb_engine[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_wb_engine[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SW_STATE_WB_ENGINE, layout_array_wb_engine, sw_state_layout_array[unit][WB_ENGINE_MODULE_ID], DNX_SW_STATE_SW_STATE_WB_ENGINE_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#undef BSL_LOG_MODULE
