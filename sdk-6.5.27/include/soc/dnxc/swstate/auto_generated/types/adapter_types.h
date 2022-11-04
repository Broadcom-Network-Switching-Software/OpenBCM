
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __ADAPTER_TYPES_H__
#define __ADAPTER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/types.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>


typedef struct {
    int adapter_rx_tx_fd;
    int adapter_mem_reg_fd;
    int adapter_sdk_interface_fd;
    int adapter_external_events_fd;
    uint32 adapter_external_events_port;
    uint32 adapter_server_address;
    sw_state_mutex_t adapter_rx_tx_mutex;
    sw_state_mutex_t adapter_mem_reg_mutex;
    sw_state_mutex_t adapter_sdk_interface_mutex;
    sw_state_mutex_t adapter_external_events_mutex;
} dnxc_adapter_context_params_t;

typedef struct {
    dnxc_adapter_context_params_t params[2];
} adapter_basic_access_info_t;


#endif 
