
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_LANE_MAP_ACCESS_H__
#define __DNX_ALGO_LANE_MAP_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_lane_map_types.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
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

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_set_cb)(
    int unit, bcm_pbmp_t logical_lanes);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_get_cb)(
    int unit, bcm_pbmp_t *logical_lanes);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_neq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_eq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_member_cb)(
    int unit, uint32 _input_port, uint8 *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_not_null_cb)(
    int unit, uint8 *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_is_null_cb)(
    int unit, uint8 *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_count_cb)(
    int unit, int *result);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_xor_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_remove_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_assign_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_get_cb)(
    int unit, _shr_pbmp_t *output_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_and_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_negate_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_or_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_clear_cb)(
    int unit);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_add_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_flip_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_remove_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_set_cb)(
    int unit, uint32 _input_port);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_ports_range_add_cb)(
    int unit, uint32 _first_port, uint32 _range);

typedef int (*dnx_algo_lane_map_db_otn_logical_lanes_pbmp_fmt_cb)(
    int unit, char* _buffer);

typedef int (*dnx_algo_lane_map_db_otn_logical_lane_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_set_cb)(
    int unit, uint32 logical_lane_map_idx_0, uint16 logical_otn_lane);

typedef int (*dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_get_cb)(
    int unit, uint32 logical_lane_map_idx_0, uint16 *logical_otn_lane);

typedef int (*dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_set_cb)(
    int unit, uint32 logical_lane_map_idx_0, uint16 nof_lanes);

typedef int (*dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_get_cb)(
    int unit, uint32 logical_lane_map_idx_0, uint16 *nof_lanes);



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
    dnx_algo_lane_map_db_otn_logical_lanes_set_cb set;
    dnx_algo_lane_map_db_otn_logical_lanes_get_cb get;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_neq_cb pbmp_neq;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_eq_cb pbmp_eq;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_member_cb pbmp_member;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_not_null_cb pbmp_not_null;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_is_null_cb pbmp_is_null;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_count_cb pbmp_count;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_xor_cb pbmp_xor;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_remove_cb pbmp_remove;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_assign_cb pbmp_assign;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_get_cb pbmp_get;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_and_cb pbmp_and;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_negate_cb pbmp_negate;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_or_cb pbmp_or;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_clear_cb pbmp_clear;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_add_cb pbmp_port_add;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_flip_cb pbmp_port_flip;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_remove_cb pbmp_port_remove;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_set_cb pbmp_port_set;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_ports_range_add_cb pbmp_ports_range_add;
    dnx_algo_lane_map_db_otn_logical_lanes_pbmp_fmt_cb pbmp_fmt;
} dnx_algo_lane_map_db_otn_logical_lanes_cbs;

typedef struct {
    dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_set_cb set;
    dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_get_cb get;
} dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_cbs;

typedef struct {
    dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_set_cb set;
    dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_get_cb get;
} dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_cbs;

typedef struct {
    dnx_algo_lane_map_db_otn_logical_lane_map_alloc_cb alloc;
    dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_cbs logical_otn_lane;
    dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_cbs nof_lanes;
} dnx_algo_lane_map_db_otn_logical_lane_map_cbs;

typedef struct {
    dnx_algo_lane_map_db_otn_logical_lanes_cbs logical_lanes;
    dnx_algo_lane_map_db_otn_logical_lane_map_cbs logical_lane_map;
} dnx_algo_lane_map_db_otn_cbs;

typedef struct {
    dnx_algo_lane_map_db_is_init_cb is_init;
    dnx_algo_lane_map_db_init_cb init;
    dnx_algo_lane_map_db_lane_map_cbs lane_map;
    dnx_algo_lane_map_db_otn_cbs otn;
} dnx_algo_lane_map_db_cbs;





extern dnx_algo_lane_map_db_cbs dnx_algo_lane_map_db;

#endif 
