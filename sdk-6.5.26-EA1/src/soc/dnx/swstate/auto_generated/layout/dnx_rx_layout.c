
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_rx_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_rx_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_rx[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO_NOF_PARAMS)];

shr_error_e
dnx_rx_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info";
    layout_array_dnx_rx[idx].type = "rx_paser_info_t";
    layout_array_dnx_rx[idx].doc = "rx parser configration information";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(rx_paser_info_t);
    layout_array_dnx_rx[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].first_child_index = 1;
    layout_array_dnx_rx[idx].last_child_index = 15;
    layout_array_dnx_rx[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__FTMH_LB_KEY_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__ftmh_lb_key_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of FTMH LB-Key extension.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 2;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__FTMH_STACKING_EXT_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__ftmh_stacking_ext_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of FTMH stacking extension.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 3;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__USER_HEADER_ENABLE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__user_header_enable";
    layout_array_dnx_rx[idx].type = "uint8";
    layout_array_dnx_rx[idx].doc = "User Defined Header(UDH) enable.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint8);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 4;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__PPH_BASE_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__pph_base_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of PPH Base Header.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 5;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__OUT_LIF_X_1_EXT_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__out_lif_x_1_ext_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of 1XOutLif type of extension.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 6;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__OUT_LIF_X_2_EXT_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__out_lif_x_2_ext_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of 2XOutLif type of extension.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 7;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__OUT_LIF_X_3_EXT_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__out_lif_x_3_ext_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of 3XOutLif type of extension.";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 8;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__IN_LIF_EXT_SIZE);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__in_lif_ext_size";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "Size of Lif extension for InLIF + InLIF-Profile";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 9;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__OAMP_SYSTEM_PORT_0);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__oamp_system_port_0";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "System port of OAMP on core 0";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 10;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__OAMP_SYSTEM_PORT_1);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__oamp_system_port_1";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "System port of OAMP on core 1";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 11;
    layout_array_dnx_rx[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__DMA_CHANNEL_TO_CPU_PORT);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__dma_channel_to_cpu_port";
    layout_array_dnx_rx[idx].type = "int*";
    layout_array_dnx_rx[idx].doc = "DMA channel to CPU ports";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(int*);
    layout_array_dnx_rx[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 12;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__CPU_PORT_TO_DMA_CHANNEL);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__cpu_port_to_dma_channel";
    layout_array_dnx_rx[idx].type = "int*";
    layout_array_dnx_rx[idx].doc = "CPU port to DMA channel";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(int*);
    layout_array_dnx_rx[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 13;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__FTMH_PP_DSP_TO_CPU_PORT);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__ftmh_pp_dsp_to_cpu_port";
    layout_array_dnx_rx[idx].type = "int*";
    layout_array_dnx_rx[idx].doc = "FTMH.PP_DSP to CPU port";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(int*);
    layout_array_dnx_rx[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 14;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__ETPP_OAM_UP_MEP_DEST_TRAP_ID1);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__etpp_oam_up_mep_dest_trap_id1";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "trap id for etpp oam up mep destination 1";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = 15;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_RX_PARSER_INFO__ETPP_OAM_UP_MEP_DEST_TRAP_ID2);
    layout_array_dnx_rx[idx].name = "dnx_rx_parser_info__etpp_oam_up_mep_dest_trap_id2";
    layout_array_dnx_rx[idx].type = "uint32";
    layout_array_dnx_rx[idx].doc = "trap id for etpp oam up mep destination 2";
    layout_array_dnx_rx[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].size_of = sizeof(uint32);
    layout_array_dnx_rx[idx].parent  = 0;
    layout_array_dnx_rx[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_rx[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_RX_PARSER_INFO, layout_array_dnx_rx, sw_state_layout_array[unit][DNX_RX_MODULE_ID], DNX_SW_STATE_DNX_RX_PARSER_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
