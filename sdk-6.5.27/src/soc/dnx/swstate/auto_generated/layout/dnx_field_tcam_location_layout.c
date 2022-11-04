
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_location_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_location[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_location_init_layout_structure(int unit)
{
    uint32 dnx_field_tcam_location_sw__location_priority_arr__default_val = DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw";
    layout_array_dnx_field_tcam_location[idx].type = "dnx_field_tcam_location_sw_t";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(dnx_field_tcam_location_sw_t);
    layout_array_dnx_field_tcam_location[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__FIRST);
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__PRIORITIES_RANGE);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__priorities_range";
    layout_array_dnx_field_tcam_location[idx].type = "sw_state_sorted_ll_t";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(sw_state_sorted_ll_t);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 0;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_EXPANDED_INTERFACE | DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__tcam_handlers_info";
    layout_array_dnx_field_tcam_location[idx].type = "dnx_field_tcam_location_handler_info_t";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(dnx_field_tcam_location_handler_info_t);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_field.tcam.nof_tcam_handlers_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 0;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__FIRST);
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__LOCATION_PRIORITY_ARR);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__location_priority_arr";
    layout_array_dnx_field_tcam_location[idx].type = "uint32";
    layout_array_dnx_field_tcam_location[idx].default_value= &(dnx_field_tcam_location_sw__location_priority_arr__default_val);
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_tcam_location[idx].array_indexes[1].num_elements = dnx_data_field.tcam.tcam_banks_size_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 0;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_BANKS_OCCUPATION_BITMAP);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__tcam_banks_occupation_bitmap";
    layout_array_dnx_field_tcam_location[idx].type = "sw_state_occ_bm_t";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(sw_state_occ_bm_t);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 0;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__ENTRIES_OCCUPATION_BITMAP);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__tcam_handlers_info__entries_occupation_bitmap";
    layout_array_dnx_field_tcam_location[idx].type = "sw_state_occ_bm_t";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(sw_state_occ_bm_t);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 2;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__PRIO_LIST_FIRST_ITER);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__tcam_handlers_info__prio_list_first_iter";
    layout_array_dnx_field_tcam_location[idx].type = "uint32";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 2;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__PRIO_LIST_LAST_ITER);
    layout_array_dnx_field_tcam_location[idx].name = "dnx_field_tcam_location_sw__tcam_handlers_info__prio_list_last_iter";
    layout_array_dnx_field_tcam_location[idx].type = "uint32";
    layout_array_dnx_field_tcam_location[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_location[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_location[idx].parent  = 2;
    layout_array_dnx_field_tcam_location[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_location[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW, layout_array_dnx_field_tcam_location, sw_state_layout_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
