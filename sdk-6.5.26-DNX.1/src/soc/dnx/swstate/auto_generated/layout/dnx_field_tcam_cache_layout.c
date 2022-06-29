
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_cache_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_cache_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_cache[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_cache_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow";
    layout_array_dnx_field_tcam_cache[idx].type = "dnx_field_tcam_cache_shadow_t";
    layout_array_dnx_field_tcam_cache[idx].doc = "Holds The MDB TCAM's shadow in SW state.";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(dnx_field_tcam_cache_shadow_t);
    layout_array_dnx_field_tcam_cache[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__FIRST);
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__tcam_bank";
    layout_array_dnx_field_tcam_cache[idx].type = "dnx_field_tcam_cache_shadow_bank_t**";
    layout_array_dnx_field_tcam_cache[idx].doc = "The TCAM shadow for each bank.                      To be allocated according to the number of banks, if TCAM shadow is in use.";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(dnx_field_tcam_cache_shadow_bank_t**);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 0;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__FIRST);
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__payload_table";
    layout_array_dnx_field_tcam_cache[idx].type = "dnx_field_tcam_cache_shadow_payload_table_t**";
    layout_array_dnx_field_tcam_cache[idx].doc = "The TCAM shadow for each bank.                      To be allocated according to the number of payload tables, if TCAM shadow is in use.";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(dnx_field_tcam_cache_shadow_payload_table_t**);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 0;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__FIRST);
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__tcam_bank__entry_key";
    layout_array_dnx_field_tcam_cache[idx].type = "dnx_field_tcam_cache_shadow_entry_t*";
    layout_array_dnx_field_tcam_cache[idx].doc = "The TCAM shadow for each line in the bank (counting key and mask as separate lines).                      Includes either key or mask, entry size and valid bits.                      To be allocated according to the bank size.";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(dnx_field_tcam_cache_shadow_entry_t*);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 1;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY__FIRST);
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY__KEY_DATA);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__tcam_bank__entry_key__key_data";
    layout_array_dnx_field_tcam_cache[idx].type = "uint8";
    layout_array_dnx_field_tcam_cache[idx].doc = "This sturct represents a shadow entry, the shadow entry consists of a single TCAM entry in HW                          (which also includes mode bits, plus two valid bits per each half entry";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 3;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__payload_table__entry_payload";
    layout_array_dnx_field_tcam_cache[idx].type = "dnx_field_tcam_cache_shadow_payload_entry_t*";
    layout_array_dnx_field_tcam_cache[idx].doc = "The TCAM shadow for each line in the payload table.                      Includes payload.                      To be allocated according to the bank size.";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(dnx_field_tcam_cache_shadow_payload_entry_t*);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 2;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD__FIRST);
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD__PAYLOAD_DATA);
    layout_array_dnx_field_tcam_cache[idx].name = "dnx_field_tcam_cache_shadow__payload_table__entry_payload__payload_data";
    layout_array_dnx_field_tcam_cache[idx].type = "uint8";
    layout_array_dnx_field_tcam_cache[idx].doc = "This struct represents a shadow payload entry, since action per bank is split between two payload                          SRAMs, the size of each shadow payload entry is equal to the size of half a TCAM action";
    layout_array_dnx_field_tcam_cache[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].num_elements = BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF);
    layout_array_dnx_field_tcam_cache[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_field_tcam_cache[idx].parent  = 5;
    layout_array_dnx_field_tcam_cache[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_cache[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ARRAY_RANGE_FUNCTIONS;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW, layout_array_dnx_field_tcam_cache, sw_state_layout_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
