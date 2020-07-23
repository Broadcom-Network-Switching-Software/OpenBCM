
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_RX_DIAGNOSTIC_H__
#define __DNX_RX_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_rx_types.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_RX_PARSER_INFO_INFO,
    DNX_RX_PARSER_INFO_FTMH_LB_KEY_SIZE_INFO,
    DNX_RX_PARSER_INFO_FTMH_STACKING_EXT_SIZE_INFO,
    DNX_RX_PARSER_INFO_USER_HEADER_ENABLE_INFO,
    DNX_RX_PARSER_INFO_PPH_BASE_SIZE_INFO,
    DNX_RX_PARSER_INFO_OUT_LIF_X_1_EXT_SIZE_INFO,
    DNX_RX_PARSER_INFO_OUT_LIF_X_2_EXT_SIZE_INFO,
    DNX_RX_PARSER_INFO_OUT_LIF_X_3_EXT_SIZE_INFO,
    DNX_RX_PARSER_INFO_IN_LIF_EXT_SIZE_INFO,
    DNX_RX_PARSER_INFO_OAMP_SYSTEM_PORT_0_INFO,
    DNX_RX_PARSER_INFO_OAMP_SYSTEM_PORT_1_INFO,
    DNX_RX_PARSER_INFO_DMA_CHANNEL_TO_CPU_PORT_INFO,
    DNX_RX_PARSER_INFO_CPU_PORT_TO_DMA_CHANNEL_INFO,
    DNX_RX_PARSER_INFO_INFO_NOF_ENTRIES
} sw_state_dnx_rx_parser_info_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_rx_parser_info_info[SOC_MAX_NUM_DEVICES][DNX_RX_PARSER_INFO_INFO_NOF_ENTRIES];

extern const char* dnx_rx_parser_info_layout_str[DNX_RX_PARSER_INFO_INFO_NOF_ENTRIES];
int dnx_rx_parser_info_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_ftmh_lb_key_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_ftmh_stacking_ext_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_user_header_enable_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_pph_base_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_out_lif_x_1_ext_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_out_lif_x_2_ext_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_out_lif_x_3_ext_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_in_lif_ext_size_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_oamp_system_port_0_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_oamp_system_port_1_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_dma_channel_to_cpu_port_dump(
    int unit, int dma_channel_to_cpu_port_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_rx_parser_info_cpu_port_to_dma_channel_dump(
    int unit, int cpu_port_to_dma_channel_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
