
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_hit_indication_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_hit_indication_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_hit_indication[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_hit_indication_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION);
    layout_array_dnx_field_tcam_hit_indication[idx].name = "dnx_field_tcam_hit_indication";
    layout_array_dnx_field_tcam_hit_indication[idx].type = "dnx_field_tcam_hit_indication_t";
    layout_array_dnx_field_tcam_hit_indication[idx].doc = "Contains information for hit indication on Big and Small banks.";
    layout_array_dnx_field_tcam_hit_indication[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].size_of = sizeof(dnx_field_tcam_hit_indication_t);
    layout_array_dnx_field_tcam_hit_indication[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__FIRST);
    layout_array_dnx_field_tcam_hit_indication[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION);
    layout_array_dnx_field_tcam_hit_indication[idx].name = "dnx_field_tcam_hit_indication__tcam_action_hit_indication";
    layout_array_dnx_field_tcam_hit_indication[idx].type = "dnx_field_tcam_hit_indication_big_banks_sram_info_t*";
    layout_array_dnx_field_tcam_hit_indication[idx].doc = "Hit indication for information for Big Banks.";
    layout_array_dnx_field_tcam_hit_indication[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].size_of = sizeof(dnx_field_tcam_hit_indication_big_banks_sram_info_t*);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_big_banks_srams_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].parent  = 0;
    layout_array_dnx_field_tcam_hit_indication[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__FIRST);
    layout_array_dnx_field_tcam_hit_indication[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION_SMALL);
    layout_array_dnx_field_tcam_hit_indication[idx].name = "dnx_field_tcam_hit_indication__tcam_action_hit_indication_small";
    layout_array_dnx_field_tcam_hit_indication[idx].type = "dnx_field_tcam_hit_indication_small_banks_sram_info_t*";
    layout_array_dnx_field_tcam_hit_indication[idx].doc = "Hit indication for information for Small Banks.";
    layout_array_dnx_field_tcam_hit_indication[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].size_of = sizeof(dnx_field_tcam_hit_indication_small_banks_sram_info_t*);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_small_banks_srams_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].parent  = 0;
    layout_array_dnx_field_tcam_hit_indication[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION_SMALL__FIRST);
    layout_array_dnx_field_tcam_hit_indication[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION_SMALL__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BANK_LINE_ID);
    layout_array_dnx_field_tcam_hit_indication[idx].name = "dnx_field_tcam_hit_indication__tcam_action_hit_indication__bank_line_id";
    layout_array_dnx_field_tcam_hit_indication[idx].type = "uint8**";
    layout_array_dnx_field_tcam_hit_indication[idx].doc = "The Big Bank line IDs, which hit_value will be stored.";
    layout_array_dnx_field_tcam_hit_indication[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].size_of = sizeof(uint8**);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[1].num_elements = dnx_data_field.tcam.nof_big_bank_lines_per_sram_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].parent  = 1;
    layout_array_dnx_field_tcam_hit_indication[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION_SMALL__BANK_LINE_ID);
    layout_array_dnx_field_tcam_hit_indication[idx].name = "dnx_field_tcam_hit_indication__tcam_action_hit_indication_small__bank_line_id";
    layout_array_dnx_field_tcam_hit_indication[idx].type = "uint8**";
    layout_array_dnx_field_tcam_hit_indication[idx].doc = "The Small Bank line IDs, which hit_value will be stored.";
    layout_array_dnx_field_tcam_hit_indication[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].size_of = sizeof(uint8**);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[1].num_elements = dnx_data_field.tcam.nof_small_bank_lines_per_sram_get(unit);
    layout_array_dnx_field_tcam_hit_indication[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_hit_indication[idx].parent  = 2;
    layout_array_dnx_field_tcam_hit_indication[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_hit_indication[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION, layout_array_dnx_field_tcam_hit_indication, sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
