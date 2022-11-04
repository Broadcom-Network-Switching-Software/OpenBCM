
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_prefix_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_prefix_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_prefix[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_prefix_init_layout_structure(int unit)
{
    uint32 dnx_field_tcam_prefix_sw__banks_prefix__core_prefix_map__prefix_handler_map__default_val = DNX_FIELD_TCAM_HANDLER_ID_INVALID;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW);
    layout_array_dnx_field_tcam_prefix[idx].name = "dnx_field_tcam_prefix_sw";
    layout_array_dnx_field_tcam_prefix[idx].type = "dnx_field_tcam_prefix_sw_t";
    layout_array_dnx_field_tcam_prefix[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].size_of = sizeof(dnx_field_tcam_prefix_sw_t);
    layout_array_dnx_field_tcam_prefix[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__FIRST);
    layout_array_dnx_field_tcam_prefix[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX);
    layout_array_dnx_field_tcam_prefix[idx].name = "dnx_field_tcam_prefix_sw__banks_prefix";
    layout_array_dnx_field_tcam_prefix[idx].type = "dnx_field_tcam_core_prefix_t";
    layout_array_dnx_field_tcam_prefix[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].size_of = sizeof(dnx_field_tcam_core_prefix_t);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_banks_get(unit);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_prefix[idx].parent  = 0;
    layout_array_dnx_field_tcam_prefix[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__FIRST);
    layout_array_dnx_field_tcam_prefix[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP);
    layout_array_dnx_field_tcam_prefix[idx].name = "dnx_field_tcam_prefix_sw__banks_prefix__core_prefix_map";
    layout_array_dnx_field_tcam_prefix[idx].type = "dnx_field_tcam_prefix_t";
    layout_array_dnx_field_tcam_prefix[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].size_of = sizeof(dnx_field_tcam_prefix_t);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_prefix[idx].parent  = 1;
    layout_array_dnx_field_tcam_prefix[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__FIRST);
    layout_array_dnx_field_tcam_prefix[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP);
    layout_array_dnx_field_tcam_prefix[idx].name = "dnx_field_tcam_prefix_sw__banks_prefix__core_prefix_map__prefix_handler_map";
    layout_array_dnx_field_tcam_prefix[idx].type = "uint32";
    layout_array_dnx_field_tcam_prefix[idx].default_value= &(dnx_field_tcam_prefix_sw__banks_prefix__core_prefix_map__prefix_handler_map__default_val);
    layout_array_dnx_field_tcam_prefix[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].num_elements = dnx_data_field.tcam.max_prefix_value_get(unit);
    layout_array_dnx_field_tcam_prefix[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_prefix[idx].parent  = 2;
    layout_array_dnx_field_tcam_prefix[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_prefix[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW, layout_array_dnx_field_tcam_prefix, sw_state_layout_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
