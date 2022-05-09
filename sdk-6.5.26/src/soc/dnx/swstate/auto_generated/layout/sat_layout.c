
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/sat_types.h>
#include <soc/dnx/swstate/auto_generated/layout/sat_layout.h>

dnxc_sw_state_layout_t layout_array_sat[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SAT_PKT_HEADER_INFO_NOF_PARAMS)];

shr_error_e
sat_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SAT_PKT_HEADER_INFO);
    layout_array_sat[idx].name = "sat_pkt_header_info";
    layout_array_sat[idx].type = "sat_pkt_header_info_t";
    layout_array_sat[idx].doc = "Map sat packet header profile to PKT_HEADER_BASE";
    layout_array_sat[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sat[idx].size_of = sizeof(sat_pkt_header_info_t);
    layout_array_sat[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sat[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SAT_PKT_HEADER_INFO__FIRST);
    layout_array_sat[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SAT_PKT_HEADER_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE);
    layout_array_sat[idx].name = "sat_pkt_header_info__pkt_header_base";
    layout_array_sat[idx].type = "uint16*";
    layout_array_sat[idx].doc = "Hold pkt_header_base for each packet header profile";
    layout_array_sat[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sat[idx].size_of = sizeof(uint16*);
    layout_array_sat[idx].array_indexes[0].num_elements = dnx_data_sat.generator.nof_gtf_ids_get(unit)+1;
    layout_array_sat[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_sat[idx].parent  = 0;
    layout_array_sat[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sat[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_sat[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_GENERATE_IS_ALLOC;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SAT_PKT_HEADER_INFO, layout_array_sat, sw_state_layout_array[unit][SAT_MODULE_ID], DNX_SW_STATE_SAT_PKT_HEADER_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
