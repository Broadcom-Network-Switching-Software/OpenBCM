
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_ACCESS_H__
#define __DNXF_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>



typedef int (*dnxf_state_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnxf_state_init_cb)(
    int unit);


typedef int (*dnxf_state_modid_local_map_set_cb)(
    int unit, uint32 local_map_idx_0, CONST soc_dnxf_modid_local_map_t *local_map);


typedef int (*dnxf_state_modid_local_map_get_cb)(
    int unit, uint32 local_map_idx_0, soc_dnxf_modid_local_map_t *local_map);


typedef int (*dnxf_state_modid_local_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_modid_local_map_valid_set_cb)(
    int unit, uint32 local_map_idx_0, uint32 valid);


typedef int (*dnxf_state_modid_local_map_valid_get_cb)(
    int unit, uint32 local_map_idx_0, uint32 *valid);


typedef int (*dnxf_state_modid_local_map_module_id_set_cb)(
    int unit, uint32 local_map_idx_0, uint32 module_id);


typedef int (*dnxf_state_modid_local_map_module_id_get_cb)(
    int unit, uint32 local_map_idx_0, uint32 *module_id);


typedef int (*dnxf_state_modid_group_map_set_cb)(
    int unit, uint32 group_map_idx_0, CONST soc_dnxf_modid_group_map_t *group_map);


typedef int (*dnxf_state_modid_group_map_get_cb)(
    int unit, uint32 group_map_idx_0, soc_dnxf_modid_group_map_t *group_map);


typedef int (*dnxf_state_modid_group_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_modid_group_map_raw_set_cb)(
    int unit, uint32 group_map_idx_0, uint32 raw_idx_0, uint32 raw);


typedef int (*dnxf_state_modid_group_map_raw_get_cb)(
    int unit, uint32 group_map_idx_0, uint32 raw_idx_0, uint32 *raw);


typedef int (*dnxf_state_modid_modid_to_group_map_set_cb)(
    int unit, uint32 modid_to_group_map_idx_0, soc_module_t modid_to_group_map);


typedef int (*dnxf_state_modid_modid_to_group_map_get_cb)(
    int unit, uint32 modid_to_group_map_idx_0, soc_module_t *modid_to_group_map);


typedef int (*dnxf_state_mc_mode_set_cb)(
    int unit, uint32 mode);


typedef int (*dnxf_state_mc_mode_get_cb)(
    int unit, uint32 *mode);


typedef int (*dnxf_state_mc_id_map_set_cb)(
    int unit, uint32 id_map_idx_0, uint32 id_map);


typedef int (*dnxf_state_mc_id_map_get_cb)(
    int unit, uint32 id_map_idx_0, uint32 *id_map);


typedef int (*dnxf_state_mc_id_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_mc_nof_mc_groups_created_set_cb)(
    int unit, uint32 nof_mc_groups_created);


typedef int (*dnxf_state_mc_nof_mc_groups_created_get_cb)(
    int unit, uint32 *nof_mc_groups_created);


typedef int (*dnxf_state_lb_group_to_first_link_set_cb)(
    int unit, uint32 group_to_first_link_idx_0, soc_port_t group_to_first_link);


typedef int (*dnxf_state_lb_group_to_first_link_get_cb)(
    int unit, uint32 group_to_first_link_idx_0, soc_port_t *group_to_first_link);


typedef int (*dnxf_state_lb_group_to_first_link_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_port_cl72_conf_set_cb)(
    int unit, uint32 cl72_conf_idx_0, uint32 cl72_conf);


typedef int (*dnxf_state_port_cl72_conf_get_cb)(
    int unit, uint32 cl72_conf_idx_0, uint32 *cl72_conf);


typedef int (*dnxf_state_port_cl72_conf_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_port_phy_lane_config_set_cb)(
    int unit, uint32 phy_lane_config_idx_0, int phy_lane_config);


typedef int (*dnxf_state_port_phy_lane_config_get_cb)(
    int unit, uint32 phy_lane_config_idx_0, int *phy_lane_config);


typedef int (*dnxf_state_port_phy_lane_config_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_port_is_connected_to_repeater_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_negate_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_null_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_test_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnxf_state_port_is_connected_to_repeater_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);


typedef int (*dnxf_state_port_isolation_status_stored_set_cb)(
    int unit, uint32 isolation_status_stored_idx_0, int isolation_status_stored);


typedef int (*dnxf_state_port_isolation_status_stored_get_cb)(
    int unit, uint32 isolation_status_stored_idx_0, int *isolation_status_stored);


typedef int (*dnxf_state_port_isolation_status_stored_inc_cb)(
    int unit, uint32 isolation_status_stored_idx_0, uint32 inc_value);


typedef int (*dnxf_state_port_isolation_status_stored_dec_cb)(
    int unit, uint32 isolation_status_stored_idx_0, uint32 dec_value);


typedef int (*dnxf_state_port_isolation_status_stored_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_port_orig_isolated_status_set_cb)(
    int unit, uint32 orig_isolated_status_idx_0, int orig_isolated_status);


typedef int (*dnxf_state_port_orig_isolated_status_get_cb)(
    int unit, uint32 orig_isolated_status_idx_0, int *orig_isolated_status);


typedef int (*dnxf_state_port_orig_isolated_status_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_intr_flags_set_cb)(
    int unit, uint32 flags_idx_0, uint32 flags);


typedef int (*dnxf_state_intr_flags_get_cb)(
    int unit, uint32 flags_idx_0, uint32 *flags);


typedef int (*dnxf_state_intr_flags_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_intr_storm_timed_count_set_cb)(
    int unit, uint32 storm_timed_count_idx_0, uint32 storm_timed_count);


typedef int (*dnxf_state_intr_storm_timed_count_get_cb)(
    int unit, uint32 storm_timed_count_idx_0, uint32 *storm_timed_count);


typedef int (*dnxf_state_intr_storm_timed_count_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_intr_storm_timed_period_set_cb)(
    int unit, uint32 storm_timed_period_idx_0, uint32 storm_timed_period);


typedef int (*dnxf_state_intr_storm_timed_period_get_cb)(
    int unit, uint32 storm_timed_period_idx_0, uint32 *storm_timed_period);


typedef int (*dnxf_state_intr_storm_timed_period_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_intr_storm_nominal_set_cb)(
    int unit, uint32 storm_nominal);


typedef int (*dnxf_state_intr_storm_nominal_get_cb)(
    int unit, uint32 *storm_nominal);


typedef int (*dnxf_state_lane_map_db_map_size_set_cb)(
    int unit, int map_size);


typedef int (*dnxf_state_lane_map_db_map_size_get_cb)(
    int unit, int *map_size);


typedef int (*dnxf_state_lane_map_db_lane2serdes_set_cb)(
    int unit, uint32 lane2serdes_idx_0, CONST soc_dnxc_lane_map_db_map_t *lane2serdes);


typedef int (*dnxf_state_lane_map_db_lane2serdes_get_cb)(
    int unit, uint32 lane2serdes_idx_0, soc_dnxc_lane_map_db_map_t *lane2serdes);


typedef int (*dnxf_state_lane_map_db_lane2serdes_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_lane_map_db_lane2serdes_rx_id_set_cb)(
    int unit, uint32 lane2serdes_idx_0, int rx_id);


typedef int (*dnxf_state_lane_map_db_lane2serdes_rx_id_get_cb)(
    int unit, uint32 lane2serdes_idx_0, int *rx_id);


typedef int (*dnxf_state_lane_map_db_lane2serdes_tx_id_set_cb)(
    int unit, uint32 lane2serdes_idx_0, int tx_id);


typedef int (*dnxf_state_lane_map_db_lane2serdes_tx_id_get_cb)(
    int unit, uint32 lane2serdes_idx_0, int *tx_id);


typedef int (*dnxf_state_lane_map_db_serdes2lane_set_cb)(
    int unit, uint32 serdes2lane_idx_0, CONST soc_dnxc_lane_map_db_map_t *serdes2lane);


typedef int (*dnxf_state_lane_map_db_serdes2lane_get_cb)(
    int unit, uint32 serdes2lane_idx_0, soc_dnxc_lane_map_db_map_t *serdes2lane);


typedef int (*dnxf_state_lane_map_db_serdes2lane_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnxf_state_lane_map_db_serdes2lane_rx_id_set_cb)(
    int unit, uint32 serdes2lane_idx_0, int rx_id);


typedef int (*dnxf_state_lane_map_db_serdes2lane_rx_id_get_cb)(
    int unit, uint32 serdes2lane_idx_0, int *rx_id);


typedef int (*dnxf_state_lane_map_db_serdes2lane_tx_id_set_cb)(
    int unit, uint32 serdes2lane_idx_0, int tx_id);


typedef int (*dnxf_state_lane_map_db_serdes2lane_tx_id_get_cb)(
    int unit, uint32 serdes2lane_idx_0, int *tx_id);


typedef int (*dnxf_state_synce_master_synce_enabled_set_cb)(
    int unit, int master_synce_enabled);


typedef int (*dnxf_state_synce_master_synce_enabled_get_cb)(
    int unit, int *master_synce_enabled);


typedef int (*dnxf_state_synce_slave_synce_enabled_set_cb)(
    int unit, int slave_synce_enabled);


typedef int (*dnxf_state_synce_slave_synce_enabled_get_cb)(
    int unit, int *slave_synce_enabled);




typedef struct {
    dnxf_state_modid_local_map_valid_set_cb set;
    dnxf_state_modid_local_map_valid_get_cb get;
} dnxf_state_modid_local_map_valid_cbs;


typedef struct {
    dnxf_state_modid_local_map_module_id_set_cb set;
    dnxf_state_modid_local_map_module_id_get_cb get;
} dnxf_state_modid_local_map_module_id_cbs;


typedef struct {
    dnxf_state_modid_local_map_set_cb set;
    dnxf_state_modid_local_map_get_cb get;
    dnxf_state_modid_local_map_alloc_cb alloc;
    
    dnxf_state_modid_local_map_valid_cbs valid;
    
    dnxf_state_modid_local_map_module_id_cbs module_id;
} dnxf_state_modid_local_map_cbs;


typedef struct {
    dnxf_state_modid_group_map_raw_set_cb set;
    dnxf_state_modid_group_map_raw_get_cb get;
} dnxf_state_modid_group_map_raw_cbs;


typedef struct {
    dnxf_state_modid_group_map_set_cb set;
    dnxf_state_modid_group_map_get_cb get;
    dnxf_state_modid_group_map_alloc_cb alloc;
    
    dnxf_state_modid_group_map_raw_cbs raw;
} dnxf_state_modid_group_map_cbs;


typedef struct {
    dnxf_state_modid_modid_to_group_map_set_cb set;
    dnxf_state_modid_modid_to_group_map_get_cb get;
} dnxf_state_modid_modid_to_group_map_cbs;


typedef struct {
    
    dnxf_state_modid_local_map_cbs local_map;
    
    dnxf_state_modid_group_map_cbs group_map;
    
    dnxf_state_modid_modid_to_group_map_cbs modid_to_group_map;
} dnxf_state_modid_cbs;


typedef struct {
    dnxf_state_mc_mode_set_cb set;
    dnxf_state_mc_mode_get_cb get;
} dnxf_state_mc_mode_cbs;


typedef struct {
    dnxf_state_mc_id_map_set_cb set;
    dnxf_state_mc_id_map_get_cb get;
    dnxf_state_mc_id_map_alloc_cb alloc;
} dnxf_state_mc_id_map_cbs;


typedef struct {
    dnxf_state_mc_nof_mc_groups_created_set_cb set;
    dnxf_state_mc_nof_mc_groups_created_get_cb get;
} dnxf_state_mc_nof_mc_groups_created_cbs;


typedef struct {
    
    dnxf_state_mc_mode_cbs mode;
    
    dnxf_state_mc_id_map_cbs id_map;
    
    dnxf_state_mc_nof_mc_groups_created_cbs nof_mc_groups_created;
} dnxf_state_mc_cbs;


typedef struct {
    dnxf_state_lb_group_to_first_link_set_cb set;
    dnxf_state_lb_group_to_first_link_get_cb get;
    dnxf_state_lb_group_to_first_link_alloc_cb alloc;
} dnxf_state_lb_group_to_first_link_cbs;


typedef struct {
    
    dnxf_state_lb_group_to_first_link_cbs group_to_first_link;
} dnxf_state_lb_cbs;


typedef struct {
    dnxf_state_port_cl72_conf_set_cb set;
    dnxf_state_port_cl72_conf_get_cb get;
    dnxf_state_port_cl72_conf_alloc_cb alloc;
} dnxf_state_port_cl72_conf_cbs;


typedef struct {
    dnxf_state_port_phy_lane_config_set_cb set;
    dnxf_state_port_phy_lane_config_get_cb get;
    dnxf_state_port_phy_lane_config_alloc_cb alloc;
} dnxf_state_port_phy_lane_config_cbs;


typedef struct {
    dnxf_state_port_is_connected_to_repeater_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_port_is_connected_to_repeater_bit_set_cb bit_set;
    dnxf_state_port_is_connected_to_repeater_bit_clear_cb bit_clear;
    dnxf_state_port_is_connected_to_repeater_bit_get_cb bit_get;
    dnxf_state_port_is_connected_to_repeater_bit_range_read_cb bit_range_read;
    dnxf_state_port_is_connected_to_repeater_bit_range_write_cb bit_range_write;
    dnxf_state_port_is_connected_to_repeater_bit_range_and_cb bit_range_and;
    dnxf_state_port_is_connected_to_repeater_bit_range_or_cb bit_range_or;
    dnxf_state_port_is_connected_to_repeater_bit_range_xor_cb bit_range_xor;
    dnxf_state_port_is_connected_to_repeater_bit_range_remove_cb bit_range_remove;
    dnxf_state_port_is_connected_to_repeater_bit_range_negate_cb bit_range_negate;
    dnxf_state_port_is_connected_to_repeater_bit_range_clear_cb bit_range_clear;
    dnxf_state_port_is_connected_to_repeater_bit_range_set_cb bit_range_set;
    dnxf_state_port_is_connected_to_repeater_bit_range_null_cb bit_range_null;
    dnxf_state_port_is_connected_to_repeater_bit_range_test_cb bit_range_test;
    dnxf_state_port_is_connected_to_repeater_bit_range_eq_cb bit_range_eq;
    dnxf_state_port_is_connected_to_repeater_bit_range_count_cb bit_range_count;
} dnxf_state_port_is_connected_to_repeater_cbs;


typedef struct {
    dnxf_state_port_isolation_status_stored_set_cb set;
    dnxf_state_port_isolation_status_stored_get_cb get;
    dnxf_state_port_isolation_status_stored_inc_cb inc;
    dnxf_state_port_isolation_status_stored_dec_cb dec;
    dnxf_state_port_isolation_status_stored_alloc_cb alloc;
} dnxf_state_port_isolation_status_stored_cbs;


typedef struct {
    dnxf_state_port_orig_isolated_status_set_cb set;
    dnxf_state_port_orig_isolated_status_get_cb get;
    dnxf_state_port_orig_isolated_status_alloc_cb alloc;
} dnxf_state_port_orig_isolated_status_cbs;


typedef struct {
    
    dnxf_state_port_cl72_conf_cbs cl72_conf;
    
    dnxf_state_port_phy_lane_config_cbs phy_lane_config;
    
    dnxf_state_port_is_connected_to_repeater_cbs is_connected_to_repeater;
    
    dnxf_state_port_isolation_status_stored_cbs isolation_status_stored;
    
    dnxf_state_port_orig_isolated_status_cbs orig_isolated_status;
} dnxf_state_port_cbs;


typedef struct {
    dnxf_state_intr_flags_set_cb set;
    dnxf_state_intr_flags_get_cb get;
    dnxf_state_intr_flags_alloc_cb alloc;
} dnxf_state_intr_flags_cbs;


typedef struct {
    dnxf_state_intr_storm_timed_count_set_cb set;
    dnxf_state_intr_storm_timed_count_get_cb get;
    dnxf_state_intr_storm_timed_count_alloc_cb alloc;
} dnxf_state_intr_storm_timed_count_cbs;


typedef struct {
    dnxf_state_intr_storm_timed_period_set_cb set;
    dnxf_state_intr_storm_timed_period_get_cb get;
    dnxf_state_intr_storm_timed_period_alloc_cb alloc;
} dnxf_state_intr_storm_timed_period_cbs;


typedef struct {
    dnxf_state_intr_storm_nominal_set_cb set;
    dnxf_state_intr_storm_nominal_get_cb get;
} dnxf_state_intr_storm_nominal_cbs;


typedef struct {
    
    dnxf_state_intr_flags_cbs flags;
    
    dnxf_state_intr_storm_timed_count_cbs storm_timed_count;
    
    dnxf_state_intr_storm_timed_period_cbs storm_timed_period;
    
    dnxf_state_intr_storm_nominal_cbs storm_nominal;
} dnxf_state_intr_cbs;


typedef struct {
    dnxf_state_lane_map_db_map_size_set_cb set;
    dnxf_state_lane_map_db_map_size_get_cb get;
} dnxf_state_lane_map_db_map_size_cbs;


typedef struct {
    dnxf_state_lane_map_db_lane2serdes_rx_id_set_cb set;
    dnxf_state_lane_map_db_lane2serdes_rx_id_get_cb get;
} dnxf_state_lane_map_db_lane2serdes_rx_id_cbs;


typedef struct {
    dnxf_state_lane_map_db_lane2serdes_tx_id_set_cb set;
    dnxf_state_lane_map_db_lane2serdes_tx_id_get_cb get;
} dnxf_state_lane_map_db_lane2serdes_tx_id_cbs;


typedef struct {
    dnxf_state_lane_map_db_lane2serdes_set_cb set;
    dnxf_state_lane_map_db_lane2serdes_get_cb get;
    dnxf_state_lane_map_db_lane2serdes_alloc_cb alloc;
    
    dnxf_state_lane_map_db_lane2serdes_rx_id_cbs rx_id;
    
    dnxf_state_lane_map_db_lane2serdes_tx_id_cbs tx_id;
} dnxf_state_lane_map_db_lane2serdes_cbs;


typedef struct {
    dnxf_state_lane_map_db_serdes2lane_rx_id_set_cb set;
    dnxf_state_lane_map_db_serdes2lane_rx_id_get_cb get;
} dnxf_state_lane_map_db_serdes2lane_rx_id_cbs;


typedef struct {
    dnxf_state_lane_map_db_serdes2lane_tx_id_set_cb set;
    dnxf_state_lane_map_db_serdes2lane_tx_id_get_cb get;
} dnxf_state_lane_map_db_serdes2lane_tx_id_cbs;


typedef struct {
    dnxf_state_lane_map_db_serdes2lane_set_cb set;
    dnxf_state_lane_map_db_serdes2lane_get_cb get;
    dnxf_state_lane_map_db_serdes2lane_alloc_cb alloc;
    
    dnxf_state_lane_map_db_serdes2lane_rx_id_cbs rx_id;
    
    dnxf_state_lane_map_db_serdes2lane_tx_id_cbs tx_id;
} dnxf_state_lane_map_db_serdes2lane_cbs;


typedef struct {
    
    dnxf_state_lane_map_db_map_size_cbs map_size;
    
    dnxf_state_lane_map_db_lane2serdes_cbs lane2serdes;
    
    dnxf_state_lane_map_db_serdes2lane_cbs serdes2lane;
} dnxf_state_lane_map_db_cbs;


typedef struct {
    dnxf_state_synce_master_synce_enabled_set_cb set;
    dnxf_state_synce_master_synce_enabled_get_cb get;
} dnxf_state_synce_master_synce_enabled_cbs;


typedef struct {
    dnxf_state_synce_slave_synce_enabled_set_cb set;
    dnxf_state_synce_slave_synce_enabled_get_cb get;
} dnxf_state_synce_slave_synce_enabled_cbs;


typedef struct {
    
    dnxf_state_synce_master_synce_enabled_cbs master_synce_enabled;
    
    dnxf_state_synce_slave_synce_enabled_cbs slave_synce_enabled;
} dnxf_state_synce_cbs;


typedef struct {
    dnxf_state_is_init_cb is_init;
    dnxf_state_init_cb init;
    
    dnxf_state_modid_cbs modid;
    
    dnxf_state_mc_cbs mc;
    
    dnxf_state_lb_cbs lb;
    
    dnxf_state_port_cbs port;
    
    dnxf_state_intr_cbs intr;
    
    dnxf_state_lane_map_db_cbs lane_map_db;
    
    dnxf_state_synce_cbs synce;
} dnxf_state_cbs;





extern dnxf_state_cbs dnxf_state;

#endif 
