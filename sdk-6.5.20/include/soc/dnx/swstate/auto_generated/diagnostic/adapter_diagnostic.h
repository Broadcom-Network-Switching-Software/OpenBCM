
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ADAPTER_DIAGNOSTIC_H__
#define __ADAPTER_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/adapter_types.h>
#include <bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    ADAPTER_CONTEXT_DB_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_FD_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_RX_TX_MUTEX_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_FD_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_MEM_REG_MUTEX_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_FD_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SDK_INTERFACE_MUTEX_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_FD_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_MUTEX_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_EXTERNAL_EVENTS_PORT_INFO,
    ADAPTER_CONTEXT_DB_PARAMS_ADAPTER_SERVER_ADDRESS_INFO,
    ADAPTER_CONTEXT_DB_INFO_NOF_ENTRIES
} sw_state_adapter_context_db_layout_e;


extern dnx_sw_state_diagnostic_info_t adapter_context_db_info[SOC_MAX_NUM_DEVICES][ADAPTER_CONTEXT_DB_INFO_NOF_ENTRIES];

extern const char* adapter_context_db_layout_str[ADAPTER_CONTEXT_DB_INFO_NOF_ENTRIES];
int adapter_context_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_rx_tx_fd_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_rx_tx_mutex_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_mem_reg_fd_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_mem_reg_mutex_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_sdk_interface_fd_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_sdk_interface_mutex_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_external_events_fd_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_external_events_mutex_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_external_events_port_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

int adapter_context_db_params_adapter_server_address_dump(
    int unit, int params_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
