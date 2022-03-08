
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_ire_packet_generator_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ire_packet_generator_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_ire_packet_generator[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO_NOF_PARAMS)];

shr_error_e
dnx_ire_packet_generator_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO);
    layout_array_dnx_ire_packet_generator[idx].name = "ire_packet_generator_info";
    layout_array_dnx_ire_packet_generator[idx].type = "dnx_ire_packet_generator_info_struct";
    layout_array_dnx_ire_packet_generator[idx].doc = "Set of DNX debug info. to be saved";
    layout_array_dnx_ire_packet_generator[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ire_packet_generator[idx].size_of = sizeof(dnx_ire_packet_generator_info_struct);
    layout_array_dnx_ire_packet_generator[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ire_packet_generator[idx].first_child_index = 1;
    layout_array_dnx_ire_packet_generator[idx].last_child_index = 1;
    layout_array_dnx_ire_packet_generator[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__IRE_DATAPATH_ENABLE_STATE);
    layout_array_dnx_ire_packet_generator[idx].name = "ire_packet_generator_info__ire_datapath_enable_state";
    layout_array_dnx_ire_packet_generator[idx].type = "uint32";
    layout_array_dnx_ire_packet_generator[idx].doc = "store the datapath enable state, before start sending IRE packets";
    layout_array_dnx_ire_packet_generator[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ire_packet_generator[idx].size_of = sizeof(uint32);
    layout_array_dnx_ire_packet_generator[idx].parent  = 0;
    layout_array_dnx_ire_packet_generator[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ire_packet_generator[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ire_packet_generator[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO, layout_array_dnx_ire_packet_generator, sw_state_layout_array[unit][DNX_IRE_PACKET_GENERATOR_MODULE_ID], DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
