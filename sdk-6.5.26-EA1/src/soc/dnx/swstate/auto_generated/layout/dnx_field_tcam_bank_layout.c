
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_bank_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_bank[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_bank_init_layout_structure(int unit)
{

    dnx_field_tcam_stage_e dnx_field_tcam_bank_sw__tcam_banks__owner_stage__default_val = DNX_FIELD_TCAM_STAGE_INVALID;
    dnx_field_tcam_bank_mode_e dnx_field_tcam_bank_sw__tcam_banks__bank_mode__default_val = DNX_FIELD_TCAM_BANK_MODE_NONE;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw";
    layout_array_dnx_field_tcam_bank[idx].type = "dnx_field_tcam_bank_sw_t";
    layout_array_dnx_field_tcam_bank[idx].doc = "Holds information about all the TCAM banks in the system";
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(dnx_field_tcam_bank_sw_t);
    layout_array_dnx_field_tcam_bank[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = 1;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = 1;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks";
    layout_array_dnx_field_tcam_bank[idx].type = "dnx_field_tcam_bank_t*";
    layout_array_dnx_field_tcam_bank[idx].doc = "Array that represents all TCAM banks in the system";
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(dnx_field_tcam_bank_t*);
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_bank[idx].parent  = 0;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = 2;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = 6;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS__OWNER_STAGE);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks__owner_stage";
    layout_array_dnx_field_tcam_bank[idx].type = "dnx_field_tcam_stage_e*";
    layout_array_dnx_field_tcam_bank[idx].doc = "Owner of this bank";
    layout_array_dnx_field_tcam_bank[idx].default_value= &(dnx_field_tcam_bank_sw__tcam_banks__owner_stage__default_val);
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(dnx_field_tcam_stage_e*);
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_bank[idx].parent  = 1;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS__ACTIVE_HANDLERS_ID);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks__active_handlers_id";
    layout_array_dnx_field_tcam_bank[idx].type = "sw_state_ll_t*";
    layout_array_dnx_field_tcam_bank[idx].doc = "Linked list that contains all handlers ids that are active on this bank";
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(sw_state_ll_t*);
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_bank[idx].parent  = 1;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS__NOF_FREE_ENTRIES);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks__nof_free_entries";
    layout_array_dnx_field_tcam_bank[idx].type = "uint32**";
    layout_array_dnx_field_tcam_bank[idx].doc = "Available half keys for this bank";
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(uint32**);
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[1].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_bank[idx].parent  = 1;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS__BANK_MODE);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks__bank_mode";
    layout_array_dnx_field_tcam_bank[idx].type = "dnx_field_tcam_bank_mode_e";
    layout_array_dnx_field_tcam_bank[idx].doc = "Bank mode, see the Enum type for more details";
    layout_array_dnx_field_tcam_bank[idx].default_value= &(dnx_field_tcam_bank_sw__tcam_banks__bank_mode__default_val);
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(dnx_field_tcam_bank_mode_e);
    layout_array_dnx_field_tcam_bank[idx].parent  = 1;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW__TCAM_BANKS__KEY_SIZE);
    layout_array_dnx_field_tcam_bank[idx].name = "dnx_field_tcam_bank_sw__tcam_banks__key_size";
    layout_array_dnx_field_tcam_bank[idx].type = "uint32";
    layout_array_dnx_field_tcam_bank[idx].doc = "Key size in bits of handlers allocated on bank per core, relevant only for mixed-ratio is not supported";
    layout_array_dnx_field_tcam_bank[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_bank[idx].parent  = 1;
    layout_array_dnx_field_tcam_bank[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_bank[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW, layout_array_dnx_field_tcam_bank, sw_state_layout_array[unit][DNX_FIELD_TCAM_BANK_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_BANK_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
