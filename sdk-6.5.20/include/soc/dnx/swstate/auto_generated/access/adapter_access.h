
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ADAPTER_ACCESS_H__
#define __ADAPTER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/adapter_types.h>
#include <bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>



typedef int (*adapter_context_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*adapter_context_db_init_cb)(
    int unit);


typedef int (*adapter_context_db_params_alloc_cb)(
    int unit);


typedef int (*adapter_context_db_params_adapter_rx_tx_fd_set_cb)(
    int unit, uint32 params_idx_0, int adapter_rx_tx_fd);


typedef int (*adapter_context_db_params_adapter_rx_tx_fd_get_cb)(
    int unit, uint32 params_idx_0, int *adapter_rx_tx_fd);


typedef int (*adapter_context_db_params_adapter_rx_tx_mutex_create_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_rx_tx_mutex_is_created_cb)(
    int unit, uint32 params_idx_0, uint8 *is_created);


typedef int (*adapter_context_db_params_adapter_rx_tx_mutex_take_cb)(
    int unit, uint32 params_idx_0, int usec);


typedef int (*adapter_context_db_params_adapter_rx_tx_mutex_give_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_mem_reg_fd_set_cb)(
    int unit, uint32 params_idx_0, int adapter_mem_reg_fd);


typedef int (*adapter_context_db_params_adapter_mem_reg_fd_get_cb)(
    int unit, uint32 params_idx_0, int *adapter_mem_reg_fd);


typedef int (*adapter_context_db_params_adapter_mem_reg_mutex_create_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_mem_reg_mutex_is_created_cb)(
    int unit, uint32 params_idx_0, uint8 *is_created);


typedef int (*adapter_context_db_params_adapter_mem_reg_mutex_take_cb)(
    int unit, uint32 params_idx_0, int usec);


typedef int (*adapter_context_db_params_adapter_mem_reg_mutex_give_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_sdk_interface_fd_set_cb)(
    int unit, uint32 params_idx_0, int adapter_sdk_interface_fd);


typedef int (*adapter_context_db_params_adapter_sdk_interface_fd_get_cb)(
    int unit, uint32 params_idx_0, int *adapter_sdk_interface_fd);


typedef int (*adapter_context_db_params_adapter_sdk_interface_mutex_create_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_sdk_interface_mutex_is_created_cb)(
    int unit, uint32 params_idx_0, uint8 *is_created);


typedef int (*adapter_context_db_params_adapter_sdk_interface_mutex_take_cb)(
    int unit, uint32 params_idx_0, int usec);


typedef int (*adapter_context_db_params_adapter_sdk_interface_mutex_give_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_external_events_fd_set_cb)(
    int unit, uint32 params_idx_0, int adapter_external_events_fd);


typedef int (*adapter_context_db_params_adapter_external_events_fd_get_cb)(
    int unit, uint32 params_idx_0, int *adapter_external_events_fd);


typedef int (*adapter_context_db_params_adapter_external_events_mutex_create_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_external_events_mutex_is_created_cb)(
    int unit, uint32 params_idx_0, uint8 *is_created);


typedef int (*adapter_context_db_params_adapter_external_events_mutex_take_cb)(
    int unit, uint32 params_idx_0, int usec);


typedef int (*adapter_context_db_params_adapter_external_events_mutex_give_cb)(
    int unit, uint32 params_idx_0);


typedef int (*adapter_context_db_params_adapter_external_events_port_set_cb)(
    int unit, uint32 params_idx_0, uint32 adapter_external_events_port);


typedef int (*adapter_context_db_params_adapter_external_events_port_get_cb)(
    int unit, uint32 params_idx_0, uint32 *adapter_external_events_port);


typedef int (*adapter_context_db_params_adapter_server_address_set_cb)(
    int unit, uint32 params_idx_0, uint32 adapter_server_address);


typedef int (*adapter_context_db_params_adapter_server_address_get_cb)(
    int unit, uint32 params_idx_0, uint32 *adapter_server_address);




typedef struct {
    adapter_context_db_params_adapter_rx_tx_fd_set_cb set;
    adapter_context_db_params_adapter_rx_tx_fd_get_cb get;
} adapter_context_db_params_adapter_rx_tx_fd_cbs;


typedef struct {
    adapter_context_db_params_adapter_rx_tx_mutex_create_cb create;
    adapter_context_db_params_adapter_rx_tx_mutex_is_created_cb is_created;
    adapter_context_db_params_adapter_rx_tx_mutex_take_cb take;
    adapter_context_db_params_adapter_rx_tx_mutex_give_cb give;
} adapter_context_db_params_adapter_rx_tx_mutex_cbs;


typedef struct {
    adapter_context_db_params_adapter_mem_reg_fd_set_cb set;
    adapter_context_db_params_adapter_mem_reg_fd_get_cb get;
} adapter_context_db_params_adapter_mem_reg_fd_cbs;


typedef struct {
    adapter_context_db_params_adapter_mem_reg_mutex_create_cb create;
    adapter_context_db_params_adapter_mem_reg_mutex_is_created_cb is_created;
    adapter_context_db_params_adapter_mem_reg_mutex_take_cb take;
    adapter_context_db_params_adapter_mem_reg_mutex_give_cb give;
} adapter_context_db_params_adapter_mem_reg_mutex_cbs;


typedef struct {
    adapter_context_db_params_adapter_sdk_interface_fd_set_cb set;
    adapter_context_db_params_adapter_sdk_interface_fd_get_cb get;
} adapter_context_db_params_adapter_sdk_interface_fd_cbs;


typedef struct {
    adapter_context_db_params_adapter_sdk_interface_mutex_create_cb create;
    adapter_context_db_params_adapter_sdk_interface_mutex_is_created_cb is_created;
    adapter_context_db_params_adapter_sdk_interface_mutex_take_cb take;
    adapter_context_db_params_adapter_sdk_interface_mutex_give_cb give;
} adapter_context_db_params_adapter_sdk_interface_mutex_cbs;


typedef struct {
    adapter_context_db_params_adapter_external_events_fd_set_cb set;
    adapter_context_db_params_adapter_external_events_fd_get_cb get;
} adapter_context_db_params_adapter_external_events_fd_cbs;


typedef struct {
    adapter_context_db_params_adapter_external_events_mutex_create_cb create;
    adapter_context_db_params_adapter_external_events_mutex_is_created_cb is_created;
    adapter_context_db_params_adapter_external_events_mutex_take_cb take;
    adapter_context_db_params_adapter_external_events_mutex_give_cb give;
} adapter_context_db_params_adapter_external_events_mutex_cbs;


typedef struct {
    adapter_context_db_params_adapter_external_events_port_set_cb set;
    adapter_context_db_params_adapter_external_events_port_get_cb get;
} adapter_context_db_params_adapter_external_events_port_cbs;


typedef struct {
    adapter_context_db_params_adapter_server_address_set_cb set;
    adapter_context_db_params_adapter_server_address_get_cb get;
} adapter_context_db_params_adapter_server_address_cbs;


typedef struct {
    adapter_context_db_params_alloc_cb alloc;
    
    adapter_context_db_params_adapter_rx_tx_fd_cbs adapter_rx_tx_fd;
    
    adapter_context_db_params_adapter_rx_tx_mutex_cbs adapter_rx_tx_mutex;
    
    adapter_context_db_params_adapter_mem_reg_fd_cbs adapter_mem_reg_fd;
    
    adapter_context_db_params_adapter_mem_reg_mutex_cbs adapter_mem_reg_mutex;
    
    adapter_context_db_params_adapter_sdk_interface_fd_cbs adapter_sdk_interface_fd;
    
    adapter_context_db_params_adapter_sdk_interface_mutex_cbs adapter_sdk_interface_mutex;
    
    adapter_context_db_params_adapter_external_events_fd_cbs adapter_external_events_fd;
    
    adapter_context_db_params_adapter_external_events_mutex_cbs adapter_external_events_mutex;
    
    adapter_context_db_params_adapter_external_events_port_cbs adapter_external_events_port;
    
    adapter_context_db_params_adapter_server_address_cbs adapter_server_address;
} adapter_context_db_params_cbs;


typedef struct {
    adapter_context_db_is_init_cb is_init;
    adapter_context_db_init_cb init;
    
    adapter_context_db_params_cbs params;
} adapter_context_db_cbs;





extern adapter_context_db_cbs adapter_context_db;

#endif 
