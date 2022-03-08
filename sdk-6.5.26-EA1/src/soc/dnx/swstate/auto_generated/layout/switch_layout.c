
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


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB);
    layout_array_switch[idx].name = "switch_db";
    layout_array_switch[idx].type = "switch_sw_state_t";
    layout_array_switch[idx].doc = "Switch related sw state";
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(switch_sw_state_t);
    layout_array_switch[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].first_child_index = 1;
    layout_array_switch[idx].last_child_index = 7;
    layout_array_switch[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION);
    layout_array_switch[idx].name = "switch_db__module_verification";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].doc = "Indication for which modules the input verification is enabled/disabled";
    layout_array_switch[idx].default_value= &(switch_db__module_verification__default_val);
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].array_indexes[0].size = bcmModuleCount;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY);
    layout_array_switch[idx].name = "switch_db__module_error_recovery";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].doc = "Indication for which modules error recovery is enabled/disabled";
    layout_array_switch[idx].default_value= &(switch_db__module_error_recovery__default_val);
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].array_indexes[0].size = bcmModuleCount;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE);
    layout_array_switch[idx].name = "switch_db__l3mcastl2_ipv4_fwd_type";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].doc = "Indication for default selection IPMC-FWD-Type - v4BRIDGE or IPV4MC";
    layout_array_switch[idx].default_value= &(switch_db__l3mcastl2_ipv4_fwd_type__default_val);
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 4;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE);
    layout_array_switch[idx].name = "switch_db__l3mcastl2_ipv6_fwd_type";
    layout_array_switch[idx].type = "uint8";
    layout_array_switch[idx].doc = "Indication for default selection IPMC-FWD-Type - v6BRIDGE or IPV6MC";
    layout_array_switch[idx].default_value= &(switch_db__l3mcastl2_ipv6_fwd_type__default_val);
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint8);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING);
    layout_array_switch[idx].name = "switch_db__header_enablers_hashing";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].doc = "Load balancing field enablers per header";
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].array_indexes[0].size = DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES;
    layout_array_switch[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 6;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED);
    layout_array_switch[idx].name = "switch_db__tunnel_route_disabled";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].doc = "routing disable";
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = 7;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED);
    layout_array_switch[idx].name = "switch_db__per_port_special_label_termination_disabled";
    layout_array_switch[idx].type = "uint32";
    layout_array_switch[idx].doc = "per port speical label termination disable";
    layout_array_switch[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].size_of = sizeof(uint32);
    layout_array_switch[idx].parent  = 0;
    layout_array_switch[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_switch[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_SWITCH_DB, layout_array_switch, sw_state_layout_array[unit][SWITCH_MODULE_ID], DNX_SW_STATE_SWITCH_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
