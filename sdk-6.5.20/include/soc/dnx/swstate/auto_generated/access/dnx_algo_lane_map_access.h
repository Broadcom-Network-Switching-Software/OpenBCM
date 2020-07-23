
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_LANE_MAP_ACCESS_H__
#define __DNX_ALGO_LANE_MAP_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_lane_map_types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>



typedef int (*dnx_algo_lane_map_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_algo_lane_map_db_init_cb)(
    int unit);


typedef int (*dnx_algo_lane_map_db_lane_map_map_size_set_cb)(
    int unit, uint32 lane_map_idx_0, int map_size);


typedef int (*dnx_algo_lane_map_db_lane_map_map_size_get_cb)(
    int unit, uint32 lane_map_idx_0, int *map_size);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, CONST soc_dnxc_lane_map_db_map_t *lane2serdes);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, soc_dnxc_lane_map_db_map_t *lane2serdes);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_alloc_cb)(
    int unit, uint32 lane_map_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int rx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int *rx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int tx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int *tx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, CONST soc_dnxc_lane_map_db_map_t *serdes2lane);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, soc_dnxc_lane_map_db_map_t *serdes2lane);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_alloc_cb)(
    int unit, uint32 lane_map_idx_0, uint32 nof_instances_to_alloc_0);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int rx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int *rx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int tx_id);


typedef int (*dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_get_cb)(
    int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int *tx_id);




typedef struct {
    dnx_algo_lane_map_db_lane_map_map_size_set_cb set;
    dnx_algo_lane_map_db_lane_map_map_size_get_cb get;
} dnx_algo_lane_map_db_lane_map_map_size_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set_cb set;
    dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_get_cb get;
} dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set_cb set;
    dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_get_cb get;
} dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_lane2serdes_set_cb set;
    dnx_algo_lane_map_db_lane_map_lane2serdes_get_cb get;
    dnx_algo_lane_map_db_lane_map_lane2serdes_alloc_cb alloc;
    
    dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_cbs rx_id;
    
    dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_cbs tx_id;
} dnx_algo_lane_map_db_lane_map_lane2serdes_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set_cb set;
    dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_get_cb get;
} dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set_cb set;
    dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_get_cb get;
} dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_cbs;


typedef struct {
    dnx_algo_lane_map_db_lane_map_serdes2lane_set_cb set;
    dnx_algo_lane_map_db_lane_map_serdes2lane_get_cb get;
    dnx_algo_lane_map_db_lane_map_serdes2lane_alloc_cb alloc;
    
    dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_cbs rx_id;
    
    dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_cbs tx_id;
} dnx_algo_lane_map_db_lane_map_serdes2lane_cbs;


typedef struct {
    
    dnx_algo_lane_map_db_lane_map_map_size_cbs map_size;
    
    dnx_algo_lane_map_db_lane_map_lane2serdes_cbs lane2serdes;
    
    dnx_algo_lane_map_db_lane_map_serdes2lane_cbs serdes2lane;
} dnx_algo_lane_map_db_lane_map_cbs;


typedef struct {
    dnx_algo_lane_map_db_is_init_cb is_init;
    dnx_algo_lane_map_db_init_cb init;
    
    dnx_algo_lane_map_db_lane_map_cbs lane_map;
} dnx_algo_lane_map_db_cbs;





extern dnx_algo_lane_map_db_cbs dnx_algo_lane_map_db;

#endif 
