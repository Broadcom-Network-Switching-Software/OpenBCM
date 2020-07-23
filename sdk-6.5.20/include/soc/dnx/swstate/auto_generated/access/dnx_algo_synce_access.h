
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_SYNCE_ACCESS_H__
#define __DNX_ALGO_SYNCE_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_synce_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>



typedef int (*dnx_algo_synce_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_algo_synce_db_init_cb)(
    int unit);


typedef int (*dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set_cb)(
    int unit, int master_synce_enabled);


typedef int (*dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get_cb)(
    int unit, int *master_synce_enabled);


typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set_cb)(
    int unit, int slave_synce_enabled);


typedef int (*dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get_cb)(
    int unit, int *slave_synce_enabled);




typedef struct {
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_cbs;


typedef struct {
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_set_cb set;
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_get_cb get;
} dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_cbs;


typedef struct {
    
    dnx_algo_synce_db_fabric_synce_status_master_synce_enabled_cbs master_synce_enabled;
    
    dnx_algo_synce_db_fabric_synce_status_slave_synce_enabled_cbs slave_synce_enabled;
} dnx_algo_synce_db_fabric_synce_status_cbs;


typedef struct {
    dnx_algo_synce_db_is_init_cb is_init;
    dnx_algo_synce_db_init_cb init;
    
    dnx_algo_synce_db_fabric_synce_status_cbs fabric_synce_status;
} dnx_algo_synce_db_cbs;





extern dnx_algo_synce_db_cbs dnx_algo_synce_db;

#endif 
