
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_flush_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_flush_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_flush[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW_NOF_PARAMS)];

shr_error_e
dnx_field_flush_init_layout_structure(int unit)
{

    dnx_field_group_t dnx_field_flush_sw__flush_profile__mapped_fg__default_val = DNX_FIELD_GROUP_INVALID;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW);
    layout_array_dnx_field_flush[idx].name = "dnx_field_flush_sw";
    layout_array_dnx_field_flush[idx].type = "dnx_field_flush_sw_t";
    layout_array_dnx_field_flush[idx].doc = "Hold the needed info to manage flush entries";
    layout_array_dnx_field_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_flush[idx].size_of = sizeof(dnx_field_flush_sw_t);
    layout_array_dnx_field_flush[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FIRST);
    layout_array_dnx_field_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE);
    layout_array_dnx_field_flush[idx].name = "dnx_field_flush_sw__flush_profile";
    layout_array_dnx_field_flush[idx].type = "dnx_field_flush_profile_info_t*";
    layout_array_dnx_field_flush[idx].doc = "Holds the field groups attached to flush profiels.";
    layout_array_dnx_field_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_flush[idx].size_of = sizeof(dnx_field_flush_profile_info_t*);
    layout_array_dnx_field_flush[idx].array_indexes[0].num_elements = dnx_data_field.exem_learn_flush_machine.nof_flush_profiles_get(unit);
    layout_array_dnx_field_flush[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_flush[idx].parent  = 0;
    layout_array_dnx_field_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__FIRST);
    layout_array_dnx_field_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_FLUSH_SW__FLUSH_PROFILE__MAPPED_FG);
    layout_array_dnx_field_flush[idx].name = "dnx_field_flush_sw__flush_profile__mapped_fg";
    layout_array_dnx_field_flush[idx].type = "dnx_field_group_t";
    layout_array_dnx_field_flush[idx].doc = "The field group the flush profile is attached to. If DNX_FIELD_GROUP_INVALID then not attached.";
    layout_array_dnx_field_flush[idx].default_value= &(dnx_field_flush_sw__flush_profile__mapped_fg__default_val);
    layout_array_dnx_field_flush[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_flush[idx].size_of = sizeof(dnx_field_group_t);
    layout_array_dnx_field_flush[idx].parent  = 1;
    layout_array_dnx_field_flush[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_flush[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_FLUSH_SW, layout_array_dnx_field_flush, sw_state_layout_array[unit][DNX_FIELD_FLUSH_MODULE_ID], DNX_SW_STATE_DNX_FIELD_FLUSH_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
