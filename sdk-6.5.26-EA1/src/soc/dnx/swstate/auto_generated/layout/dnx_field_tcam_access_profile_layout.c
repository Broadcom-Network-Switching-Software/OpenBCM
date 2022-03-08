
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_profile_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_profile_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_access_profile[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_access_profile_init_layout_structure(int unit)
{

    dnx_field_key_length_type_e dnx_field_tcam_access_profile_sw__access_profiles__key_size__default_val = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    dnx_field_action_length_type_e dnx_field_tcam_access_profile_sw__access_profiles__action_size__default_val = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw";
    layout_array_dnx_field_tcam_access_profile[idx].type = "dnx_field_tcam_access_profile_sw_t";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Holds information about all the TCAM access profiles in the system";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(dnx_field_tcam_access_profile_sw_t);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = 1;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = 1;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles";
    layout_array_dnx_field_tcam_access_profile[idx].type = "dnx_field_tcam_access_profile_t*";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Array that represents all TCAM access profiles in the system";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(dnx_field_tcam_access_profile_t*);
    layout_array_dnx_field_tcam_access_profile[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 0;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = 2;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = 8;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__OCCUPIED);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__occupied";
    layout_array_dnx_field_tcam_access_profile[idx].type = "uint8";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Whether this access profile is taken or not";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__KEY_SIZE);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__key_size";
    layout_array_dnx_field_tcam_access_profile[idx].type = "dnx_field_key_length_type_e";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Key size of this access profile";
    layout_array_dnx_field_tcam_access_profile[idx].default_value= &(dnx_field_tcam_access_profile_sw__access_profiles__key_size__default_val);
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(dnx_field_key_length_type_e);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__ACTION_SIZE);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__action_size";
    layout_array_dnx_field_tcam_access_profile[idx].type = "dnx_field_action_length_type_e";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Action size of this access profile";
    layout_array_dnx_field_tcam_access_profile[idx].default_value= &(dnx_field_tcam_access_profile_sw__access_profiles__action_size__default_val);
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(dnx_field_action_length_type_e);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__BANK_IDS_BMP);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__bank_ids_bmp";
    layout_array_dnx_field_tcam_access_profile[idx].type = "uint16*";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Array of bitmap for ids of banks that this access profile uses per core";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(uint16*);
    layout_array_dnx_field_tcam_access_profile[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__IS_DIRECT);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__is_direct";
    layout_array_dnx_field_tcam_access_profile[idx].type = "uint8";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "Whether this access profile is direct_tcam or not";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 7;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__PREFIX);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__prefix";
    layout_array_dnx_field_tcam_access_profile[idx].type = "uint8";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "The access profile prefix";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW__ACCESS_PROFILES__HANDLER_ID);
    layout_array_dnx_field_tcam_access_profile[idx].name = "dnx_field_tcam_access_profile_sw__access_profiles__handler_id";
    layout_array_dnx_field_tcam_access_profile[idx].type = "uint32";
    layout_array_dnx_field_tcam_access_profile[idx].doc = "The TCAM handler id that is occupying this access profile";
    layout_array_dnx_field_tcam_access_profile[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_access_profile[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_profile[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_profile[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW, layout_array_dnx_field_tcam_access_profile, sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_PROFILE_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_PROFILE_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
