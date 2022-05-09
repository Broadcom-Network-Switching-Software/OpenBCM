
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_access_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_field_tcam_access[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW_NOF_PARAMS)];

shr_error_e
dnx_field_tcam_access_init_layout_structure(int unit)
{

    int dnx_field_tcam_access_sw__fg_params__dt_bank_id__default_val = DNX_FIELD_TCAM_BANK_ID_INVALID;
    dnx_field_tcam_stage_e dnx_field_tcam_access_sw__fg_params__stage__default_val = DNX_FIELD_TCAM_STAGE_INVALID;
    dnx_field_tcam_bank_allocation_mode_e dnx_field_tcam_access_sw__fg_params__bank_allocation_mode__default_val = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_INVALID;
    dnx_field_tcam_context_sharing_handlers_get_p dnx_field_tcam_access_sw__fg_params__context_sharing_handlers_cb__default_val = NULL;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_access_t";
    layout_array_dnx_field_tcam_access[idx].doc = "Contains information about each entry handler (access id)\n             and where should it be mapped to";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_access_t);
    layout_array_dnx_field_tcam_access[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FIRST);
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_LOCATION_HASH);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__entry_location_hash";
    layout_array_dnx_field_tcam_access[idx].type = "sw_state_idx_htbl_t*";
    layout_array_dnx_field_tcam_access[idx].doc = "Hash for mapping entry id to entry location inside TCAM";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(sw_state_idx_htbl_t*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__valid_bmp";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_access_v_bit_t*";
    layout_array_dnx_field_tcam_access[idx].doc = "Valid bitmap for all entries in TCAM per core";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_access_v_bit_t*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__FIRST);
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__entry_in_use_bmp";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_access_v_bit_t*";
    layout_array_dnx_field_tcam_access[idx].doc = "Specifies whether entry is in use";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_access_v_bit_t*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__FIRST);
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_access_fg_params_t*";
    layout_array_dnx_field_tcam_access[idx].doc = "Array that maps field group id to its related parameters";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_access_fg_params_t*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_field.group.nof_fgs_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__FIRST);
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__LAST);
    layout_array_dnx_field_tcam_access[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PREFIX_PER_CORE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_prefix_per_core";
    layout_array_dnx_field_tcam_access[idx].type = "uint32**";
    layout_array_dnx_field_tcam_access[idx].doc = "Value of prefix per core per  FG";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint32**);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_field.group.nof_fgs_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].array_indexes[1].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__CACHE_INSTALL_IN_PROGRESS);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__cache_install_in_progress";
    layout_array_dnx_field_tcam_access[idx].type = "uint8";
    layout_array_dnx_field_tcam_access[idx].doc = "Cache install in progress flag indication";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_access[idx].parent  = 0;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__VALID_BMP__V_BIT);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__valid_bmp__v_bit";
    layout_array_dnx_field_tcam_access[idx].type = "SHR_BITDCL*";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_field.tcam.tcam_banks_size_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_field_tcam_access[idx].parent  = 2;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__ENTRY_IN_USE_BMP__V_BIT);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__entry_in_use_bmp__v_bit";
    layout_array_dnx_field_tcam_access[idx].type = "SHR_BITDCL*";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].num_elements = dnx_data_field.tcam.tcam_banks_size_get(unit);
    layout_array_dnx_field_tcam_access[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_field_tcam_access[idx].parent  = 3;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DT_BANK_ID);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__dt_bank_id";
    layout_array_dnx_field_tcam_access[idx].type = "int";
    layout_array_dnx_field_tcam_access[idx].doc = "Bank id for DT FG";
    layout_array_dnx_field_tcam_access[idx].default_value= &(dnx_field_tcam_access_sw__fg_params__dt_bank_id__default_val);
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(int);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__STAGE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__stage";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_stage_e";
    layout_array_dnx_field_tcam_access[idx].doc = "The stage of this field group";
    layout_array_dnx_field_tcam_access[idx].default_value= &(dnx_field_tcam_access_sw__fg_params__stage__default_val);
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_stage_e);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__BANK_ALLOCATION_MODE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__bank_allocation_mode";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_bank_allocation_mode_e";
    layout_array_dnx_field_tcam_access[idx].doc = "Bank Allocation Mode for the FG (see dnx_field_bank_allocation_mode_e";
    layout_array_dnx_field_tcam_access[idx].default_value= &(dnx_field_tcam_access_sw__fg_params__bank_allocation_mode__default_val);
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_bank_allocation_mode_e);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__KEY_SIZE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__key_size";
    layout_array_dnx_field_tcam_access[idx].type = "uint32";
    layout_array_dnx_field_tcam_access[idx].doc = "The key size of this field group";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTION_SIZE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__action_size";
    layout_array_dnx_field_tcam_access[idx].type = "uint32";
    layout_array_dnx_field_tcam_access[idx].doc = "The action size of this field group in bits";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__ACTUAL_ACTION_SIZE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__actual_action_size";
    layout_array_dnx_field_tcam_access[idx].type = "uint32";
    layout_array_dnx_field_tcam_access[idx].doc = "The actual number of bits used in action";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__PREFIX_SIZE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__prefix_size";
    layout_array_dnx_field_tcam_access[idx].type = "uint32";
    layout_array_dnx_field_tcam_access[idx].doc = "Size of prefix";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint32);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__DIRECT_TABLE);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__direct_table";
    layout_array_dnx_field_tcam_access[idx].type = "uint8";
    layout_array_dnx_field_tcam_access[idx].doc = "Flag for direct table groups";
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(uint8);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW__FG_PARAMS__CONTEXT_SHARING_HANDLERS_CB);
    layout_array_dnx_field_tcam_access[idx].name = "dnx_field_tcam_access_sw__fg_params__context_sharing_handlers_cb";
    layout_array_dnx_field_tcam_access[idx].type = "dnx_field_tcam_context_sharing_handlers_get_p";
    layout_array_dnx_field_tcam_access[idx].doc = "Handlers in same context callback function, used when allocating a new bank";
    layout_array_dnx_field_tcam_access[idx].default_value= &(dnx_field_tcam_access_sw__fg_params__context_sharing_handlers_cb__default_val);
    layout_array_dnx_field_tcam_access[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].size_of = sizeof(dnx_field_tcam_context_sharing_handlers_get_p);
    layout_array_dnx_field_tcam_access[idx].parent  = 4;
    layout_array_dnx_field_tcam_access[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_field_tcam_access[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW, layout_array_dnx_field_tcam_access, sw_state_layout_array[unit][DNX_FIELD_TCAM_ACCESS_MODULE_ID], DNX_SW_STATE_DNX_FIELD_TCAM_ACCESS_SW_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
