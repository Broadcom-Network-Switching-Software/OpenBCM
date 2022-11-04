
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/switch_types.h>
#include <soc/dnx/swstate/auto_generated/layout/switch_layout.h>

dnxc_sw_state_layout_t layout_array_switch[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB_NOF_PARAMS)];

shr_error_e
switch_init_layout_structure(int unit)
{
    uint8 switch_db__module_verification__default_val = TRUE;
    uint8 switch_db__module_error_recovery__default_val = TRUE;
    uint8 switch_db__l3mcastl2_ipv4_fwd_type__default_val = FALSE;
    uint8 switch_db__l3mcastl2_ipv6_fwd_type__default_val = FALSE;
    uint32 switch_db__tunnel_route_disabled__default_val = 1;

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB);
    layout_array_switch[idx].name = "switch_db";
    layout_array_switch[idx].type = "switch_sw_state_t";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(switch_sw_state_t);
    layout_array_switch[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__FIRST);
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION);
    layout_array_switch[idx].name = "switch_db__module_verification";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].default_value= &(switch_db__module_verification__default_val);
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].array_indexes[0].num_elements = bcmModuleCount;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY);
    layout_array_switch[idx].name = "switch_db__module_error_recovery";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].default_value= &(switch_db__module_error_recovery__default_val);
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].array_indexes[0].num_elements = bcmModuleCount;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE);
    layout_array_switch[idx].name = "switch_db__l3mcastl2_ipv4_fwd_type";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].default_value= &(switch_db__l3mcastl2_ipv4_fwd_type__default_val);
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE);
    layout_array_switch[idx].name = "switch_db__l3mcastl2_ipv6_fwd_type";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].default_value= &(switch_db__l3mcastl2_ipv6_fwd_type__default_val);
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING);
    layout_array_switch[idx].name = "switch_db__header_enablers_hashing";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].array_indexes[0].num_elements = DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED);
    layout_array_switch[idx].name = "switch_db__tunnel_route_disabled";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].default_value= &(switch_db__tunnel_route_disabled__default_val);
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__WIDE_DATA_EXTENSION_KEY_MODE);
    layout_array_switch[idx].name = "switch_db__wide_data_extension_key_mode";
    layout_array_switch[idx].type = "int";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(int);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__WIDE_DATA_EXTENSION_KEY_FFC_BASE_OFFSET);
    layout_array_switch[idx].name = "switch_db__wide_data_extension_key_ffc_base_offset";
    layout_array_switch[idx].type = "int";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(int);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED);
    layout_array_switch[idx].name = "switch_db__per_port_special_label_termination_disabled";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__PREDEFINED_MPLS_SPECIAL_LABEL_BITMAP);
    layout_array_switch[idx].name = "switch_db__predefined_mpls_special_label_bitmap";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SWITCH_DB, layout_array_switch, sw_state_layout_array[unit][SWITCH_MODULE_ID], DNX_SW_STATE_SWITCH_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
