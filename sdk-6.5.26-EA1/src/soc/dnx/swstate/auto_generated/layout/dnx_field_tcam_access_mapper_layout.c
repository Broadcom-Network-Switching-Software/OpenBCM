
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_mapper_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_mapper_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_access_mapper[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_access_mapper_init_layout_structure(int unit)
{

    uint32 dnx_field_tcam_access_mapper_sw__key_2_location_hash__head__default_val = DNX_FIELD_TCAM_ACCESS_HASH_NULL;
    uint32 dnx_field_tcam_access_mapper_sw__key_2_location_hash__next__default_val = DNX_FIELD_TCAM_ACCESS_HASH_NULL;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW);
    layout_array_dnx_field_tcam_access_mapper[idx].name = "dnx_field_tcam_access_mapper_sw";
    layout_array_dnx_field_tcam_access_mapper[idx].type = "dnx_field_tcam_access_mapper_t";
    layout_array_dnx_field_tcam_access_mapper[idx].doc = "Maps tcam keys to location";
    layout_array_dnx_field_tcam_access_mapper[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].size_of = sizeof(dnx_field_tcam_access_mapper_t);
    layout_array_dnx_field_tcam_access_mapper[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].first_child_index = 1;
    layout_array_dnx_field_tcam_access_mapper[idx].last_child_index = 1;
    layout_array_dnx_field_tcam_access_mapper[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW__KEY_2_LOCATION_HASH);
    layout_array_dnx_field_tcam_access_mapper[idx].name = "dnx_field_tcam_access_mapper_sw__key_2_location_hash";
    layout_array_dnx_field_tcam_access_mapper[idx].type = "dnx_field_tcam_access_mapper_hash**";
    layout_array_dnx_field_tcam_access_mapper[idx].doc = "The hash that maps the key to location";
    layout_array_dnx_field_tcam_access_mapper[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].size_of = sizeof(dnx_field_tcam_access_mapper_hash**);
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access_mapper[idx].parent  = 0;
    layout_array_dnx_field_tcam_access_mapper[idx].first_child_index = 2;
    layout_array_dnx_field_tcam_access_mapper[idx].last_child_index = 3;
    layout_array_dnx_field_tcam_access_mapper[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW__KEY_2_LOCATION_HASH__HEAD);
    layout_array_dnx_field_tcam_access_mapper[idx].name = "dnx_field_tcam_access_mapper_sw__key_2_location_hash__head";
    layout_array_dnx_field_tcam_access_mapper[idx].type = "uint32*";
    layout_array_dnx_field_tcam_access_mapper[idx].default_value= &(dnx_field_tcam_access_mapper_sw__key_2_location_hash__head__default_val);
    layout_array_dnx_field_tcam_access_mapper[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].size_of = sizeof(uint32*);
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_access_mapper[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_mapper[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW__KEY_2_LOCATION_HASH__NEXT);
    layout_array_dnx_field_tcam_access_mapper[idx].name = "dnx_field_tcam_access_mapper_sw__key_2_location_hash__next";
    layout_array_dnx_field_tcam_access_mapper[idx].type = "uint32*";
    layout_array_dnx_field_tcam_access_mapper[idx].default_value= &(dnx_field_tcam_access_mapper_sw__key_2_location_hash__next__default_val);
    layout_array_dnx_field_tcam_access_mapper[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].size_of = sizeof(uint32*);
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_access_mapper[idx].parent  = 1;
    layout_array_dnx_field_tcam_access_mapper[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access_mapper[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW, layout_array_dnx_field_tcam_access_mapper, sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_MAPPER_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
