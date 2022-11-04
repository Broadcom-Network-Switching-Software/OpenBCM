
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_ACCESS_H__
#define __DNXF_ACCESS_H__

#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <include/bcm/port.h>
#include <include/bcm/switch.h>
#include <include/shared/fabric.h>
#include <include/shared/pkt.h>
#include <include/soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <include/soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


typedef int (*dnxf_state_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnxf_state_init_cb)(
    int unit);

typedef int (*dnxf_state_info_active_core_bmp_get_cb)(
    int unit, CONST SHR_BITDCL **active_core_bmp);

typedef int (*dnxf_state_info_active_core_bmp_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);

typedef int (*dnxf_state_info_active_core_bmp_bit_set_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_info_active_core_bmp_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);

typedef int (*dnxf_state_info_active_core_bmp_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);

typedef int (*dnxf_state_info_active_core_bmp_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);

typedef int (*dnxf_state_info_valid_fsrd_bmp_get_cb)(
    int unit, CONST SHR_BITDCL **valid_fsrd_bmp);

typedef int (*dnxf_state_info_valid_fsrd_bmp_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);

typedef int (*dnxf_state_info_valid_fsrd_bmp_bit_set_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_info_valid_fsrd_bmp_bit_clear_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_info_valid_fsrd_bmp_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);

typedef int (*dnxf_state_info_valid_fsrd_bmp_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);

typedef int (*dnxf_state_info_valid_fsrd_bmp_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_get_cb)(
    int unit, CONST SHR_BITDCL **valid_dch_dcml_core_bmp);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_bit_set_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_bit_clear_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);

typedef int (*dnxf_state_info_valid_dch_dcml_core_bmp_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);

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

typedef int (*dnxf_state_modid_group_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_modid_group_map_raw_alloc_bitmap_cb)(
    int unit, uint32 group_map_idx_0, uint32 _nof_bits_to_alloc);

typedef int (*dnxf_state_modid_group_map_raw_bit_set_cb)(
    int unit, uint32 group_map_idx_0, uint32 _bit_num);

typedef int (*dnxf_state_modid_group_map_raw_bit_range_read_cb)(
    int unit, uint32 group_map_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);

typedef int (*dnxf_state_modid_group_map_raw_bit_range_write_cb)(
    int unit, uint32 group_map_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);

typedef int (*dnxf_state_modid_group_map_raw_bit_range_clear_cb)(
    int unit, uint32 group_map_idx_0, uint32 _first, uint32 _count);

typedef int (*dnxf_state_modid_modid_to_group_map_set_cb)(
    int unit, uint32 modid_to_group_map_idx_0, soc_module_t modid_to_group_map);

typedef int (*dnxf_state_modid_modid_to_group_map_get_cb)(
    int unit, uint32 modid_to_group_map_idx_0, soc_module_t *modid_to_group_map);

typedef int (*dnxf_state_modid_modid_to_group_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

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
    int unit, uint32 group_to_first_link_idx_0, uint32 group_to_first_link_idx_1, soc_port_t group_to_first_link);

typedef int (*dnxf_state_lb_group_to_first_link_get_cb)(
    int unit, uint32 group_to_first_link_idx_0, uint32 group_to_first_link_idx_1, soc_port_t *group_to_first_link);

typedef int (*dnxf_state_lb_group_to_first_link_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1);

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

typedef int (*dnxf_state_port_speed_set_cb)(
    int unit, uint32 speed_idx_0, int speed);

typedef int (*dnxf_state_port_speed_get_cb)(
    int unit, uint32 speed_idx_0, int *speed);

typedef int (*dnxf_state_port_speed_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_port_fec_type_set_cb)(
    int unit, uint32 fec_type_idx_0, int fec_type);

typedef int (*dnxf_state_port_fec_type_get_cb)(
    int unit, uint32 fec_type_idx_0, int *fec_type);

typedef int (*dnxf_state_port_fec_type_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_port_is_connected_to_repeater_alloc_bitmap_cb)(
    int unit, uint32 _nof_bits_to_alloc);

typedef int (*dnxf_state_port_is_connected_to_repeater_bit_set_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_port_is_connected_to_repeater_bit_clear_cb)(
    int unit, uint32 _bit_num);

typedef int (*dnxf_state_port_is_connected_to_repeater_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);

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

typedef int (*dnxf_state_cable_swap_is_enable_set_cb)(
    int unit, uint32 is_enable_idx_0, int is_enable);

typedef int (*dnxf_state_cable_swap_is_enable_get_cb)(
    int unit, uint32 is_enable_idx_0, int *is_enable);

typedef int (*dnxf_state_cable_swap_is_enable_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_cable_swap_is_master_set_cb)(
    int unit, uint32 is_master_idx_0, int is_master);

typedef int (*dnxf_state_cable_swap_is_master_get_cb)(
    int unit, uint32 is_master_idx_0, int *is_master);

typedef int (*dnxf_state_cable_swap_is_master_alloc_cb)(
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

typedef int (*dnxf_state_lane_map_db_serdes_map_map_size_set_cb)(
    int unit, int map_size);

typedef int (*dnxf_state_lane_map_db_serdes_map_map_size_get_cb)(
    int unit, int *map_size);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_set_cb)(
    int unit, uint32 lane2serdes_idx_0, CONST soc_dnxc_lane_map_db_map_t *lane2serdes);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_get_cb)(
    int unit, uint32 lane2serdes_idx_0, soc_dnxc_lane_map_db_map_t *lane2serdes);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_set_cb)(
    int unit, uint32 lane2serdes_idx_0, int rx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_get_cb)(
    int unit, uint32 lane2serdes_idx_0, int *rx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_set_cb)(
    int unit, uint32 lane2serdes_idx_0, int tx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_get_cb)(
    int unit, uint32 lane2serdes_idx_0, int *tx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_set_cb)(
    int unit, uint32 serdes2lane_idx_0, CONST soc_dnxc_lane_map_db_map_t *serdes2lane);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_get_cb)(
    int unit, uint32 serdes2lane_idx_0, soc_dnxc_lane_map_db_map_t *serdes2lane);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_set_cb)(
    int unit, uint32 serdes2lane_idx_0, int rx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_get_cb)(
    int unit, uint32 serdes2lane_idx_0, int *rx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_set_cb)(
    int unit, uint32 serdes2lane_idx_0, int tx_id);

typedef int (*dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_get_cb)(
    int unit, uint32 serdes2lane_idx_0, int *tx_id);

typedef int (*dnxf_state_lane_map_db_link_to_fmac_lane_map_set_cb)(
    int unit, uint32 link_to_fmac_lane_map_idx_0, int link_to_fmac_lane_map);

typedef int (*dnxf_state_lane_map_db_link_to_fmac_lane_map_get_cb)(
    int unit, uint32 link_to_fmac_lane_map_idx_0, int *link_to_fmac_lane_map);

typedef int (*dnxf_state_lane_map_db_link_to_fmac_lane_map_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnxf_state_synce_master_synce_enabled_set_cb)(
    int unit, int master_synce_enabled);

typedef int (*dnxf_state_synce_master_synce_enabled_get_cb)(
    int unit, int *master_synce_enabled);

typedef int (*dnxf_state_synce_slave_synce_enabled_set_cb)(
    int unit, int slave_synce_enabled);

typedef int (*dnxf_state_synce_slave_synce_enabled_get_cb)(
    int unit, int *slave_synce_enabled);

typedef int (*dnxf_state_synce_master_set1_synce_enabled_set_cb)(
    int unit, int master_set1_synce_enabled);

typedef int (*dnxf_state_synce_master_set1_synce_enabled_get_cb)(
    int unit, int *master_set1_synce_enabled);

typedef int (*dnxf_state_synce_slave_set1_synce_enabled_set_cb)(
    int unit, int slave_set1_synce_enabled);

typedef int (*dnxf_state_synce_slave_set1_synce_enabled_get_cb)(
    int unit, int *slave_set1_synce_enabled);

typedef int (*dnxf_state_system_upgrade_state_0_set_cb)(
    int unit, uint32 state_0);

typedef int (*dnxf_state_system_upgrade_state_0_get_cb)(
    int unit, uint32 *state_0);

typedef int (*dnxf_state_system_upgrade_state_1_set_cb)(
    int unit, uint32 state_1);

typedef int (*dnxf_state_system_upgrade_state_1_get_cb)(
    int unit, uint32 *state_1);

typedef int (*dnxf_state_system_upgrade_state_2_set_cb)(
    int unit, uint32 state_2);

typedef int (*dnxf_state_system_upgrade_state_2_get_cb)(
    int unit, uint32 *state_2);

typedef int (*dnxf_state_hard_reset_callback_set_cb)(
    int unit, bcm_switch_hard_reset_callback_t callback);

typedef int (*dnxf_state_hard_reset_callback_get_cb)(
    int unit, bcm_switch_hard_reset_callback_t *callback);

typedef int (*dnxf_state_hard_reset_userdata_set_cb)(
    int unit, uint32* userdata);

typedef int (*dnxf_state_hard_reset_userdata_get_cb)(
    int unit, uint32* *userdata);

typedef int (*dnxf_state_mib_counter_pbmp_set_cb)(
    int unit, bcm_pbmp_t counter_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_get_cb)(
    int unit, bcm_pbmp_t *counter_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_neq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_eq_cb)(
    int unit, _shr_pbmp_t input_pbmp, uint8 *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_member_cb)(
    int unit, uint32 _input_port, uint8 *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_not_null_cb)(
    int unit, uint8 *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_is_null_cb)(
    int unit, uint8 *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_count_cb)(
    int unit, int *result);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_xor_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_remove_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_assign_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_get_cb)(
    int unit, _shr_pbmp_t *output_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_and_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_negate_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_or_cb)(
    int unit, _shr_pbmp_t input_pbmp);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_clear_cb)(
    int unit);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_port_add_cb)(
    int unit, uint32 _input_port);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_port_flip_cb)(
    int unit, uint32 _input_port);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_port_remove_cb)(
    int unit, uint32 _input_port);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_port_set_cb)(
    int unit, uint32 _input_port);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_ports_range_add_cb)(
    int unit, uint32 _first_port, uint32 _range);

typedef int (*dnxf_state_mib_counter_pbmp_pbmp_fmt_cb)(
    int unit, char* _buffer);

typedef int (*dnxf_state_mib_interval_set_cb)(
    int unit, int interval);

typedef int (*dnxf_state_mib_interval_get_cb)(
    int unit, int *interval);



typedef struct {
    dnxf_state_info_active_core_bmp_get_cb get;
    dnxf_state_info_active_core_bmp_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_info_active_core_bmp_bit_set_cb bit_set;
    dnxf_state_info_active_core_bmp_bit_get_cb bit_get;
    dnxf_state_info_active_core_bmp_bit_range_clear_cb bit_range_clear;
    dnxf_state_info_active_core_bmp_bit_range_set_cb bit_range_set;
} dnxf_state_info_active_core_bmp_cbs;

typedef struct {
    dnxf_state_info_valid_fsrd_bmp_get_cb get;
    dnxf_state_info_valid_fsrd_bmp_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_info_valid_fsrd_bmp_bit_set_cb bit_set;
    dnxf_state_info_valid_fsrd_bmp_bit_clear_cb bit_clear;
    dnxf_state_info_valid_fsrd_bmp_bit_get_cb bit_get;
    dnxf_state_info_valid_fsrd_bmp_bit_range_clear_cb bit_range_clear;
    dnxf_state_info_valid_fsrd_bmp_bit_range_set_cb bit_range_set;
} dnxf_state_info_valid_fsrd_bmp_cbs;

typedef struct {
    dnxf_state_info_valid_dch_dcml_core_bmp_get_cb get;
    dnxf_state_info_valid_dch_dcml_core_bmp_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_info_valid_dch_dcml_core_bmp_bit_set_cb bit_set;
    dnxf_state_info_valid_dch_dcml_core_bmp_bit_clear_cb bit_clear;
    dnxf_state_info_valid_dch_dcml_core_bmp_bit_get_cb bit_get;
    dnxf_state_info_valid_dch_dcml_core_bmp_bit_range_clear_cb bit_range_clear;
    dnxf_state_info_valid_dch_dcml_core_bmp_bit_range_set_cb bit_range_set;
} dnxf_state_info_valid_dch_dcml_core_bmp_cbs;

typedef struct {
    dnxf_state_info_active_core_bmp_cbs active_core_bmp;
    dnxf_state_info_valid_fsrd_bmp_cbs valid_fsrd_bmp;
    dnxf_state_info_valid_dch_dcml_core_bmp_cbs valid_dch_dcml_core_bmp;
} dnxf_state_info_cbs;

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
    dnxf_state_modid_group_map_raw_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_modid_group_map_raw_bit_set_cb bit_set;
    dnxf_state_modid_group_map_raw_bit_range_read_cb bit_range_read;
    dnxf_state_modid_group_map_raw_bit_range_write_cb bit_range_write;
    dnxf_state_modid_group_map_raw_bit_range_clear_cb bit_range_clear;
} dnxf_state_modid_group_map_raw_cbs;

typedef struct {
    dnxf_state_modid_group_map_alloc_cb alloc;
    dnxf_state_modid_group_map_raw_cbs raw;
} dnxf_state_modid_group_map_cbs;

typedef struct {
    dnxf_state_modid_modid_to_group_map_set_cb set;
    dnxf_state_modid_modid_to_group_map_get_cb get;
    dnxf_state_modid_modid_to_group_map_alloc_cb alloc;
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
    dnxf_state_port_speed_set_cb set;
    dnxf_state_port_speed_get_cb get;
    dnxf_state_port_speed_alloc_cb alloc;
} dnxf_state_port_speed_cbs;

typedef struct {
    dnxf_state_port_fec_type_set_cb set;
    dnxf_state_port_fec_type_get_cb get;
    dnxf_state_port_fec_type_alloc_cb alloc;
} dnxf_state_port_fec_type_cbs;

typedef struct {
    dnxf_state_port_is_connected_to_repeater_alloc_bitmap_cb alloc_bitmap;
    dnxf_state_port_is_connected_to_repeater_bit_set_cb bit_set;
    dnxf_state_port_is_connected_to_repeater_bit_clear_cb bit_clear;
    dnxf_state_port_is_connected_to_repeater_bit_get_cb bit_get;
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
    dnxf_state_port_speed_cbs speed;
    dnxf_state_port_fec_type_cbs fec_type;
    dnxf_state_port_is_connected_to_repeater_cbs is_connected_to_repeater;
    dnxf_state_port_isolation_status_stored_cbs isolation_status_stored;
    dnxf_state_port_orig_isolated_status_cbs orig_isolated_status;
} dnxf_state_port_cbs;

typedef struct {
    dnxf_state_cable_swap_is_enable_set_cb set;
    dnxf_state_cable_swap_is_enable_get_cb get;
    dnxf_state_cable_swap_is_enable_alloc_cb alloc;
} dnxf_state_cable_swap_is_enable_cbs;

typedef struct {
    dnxf_state_cable_swap_is_master_set_cb set;
    dnxf_state_cable_swap_is_master_get_cb get;
    dnxf_state_cable_swap_is_master_alloc_cb alloc;
} dnxf_state_cable_swap_is_master_cbs;

typedef struct {
    dnxf_state_cable_swap_is_enable_cbs is_enable;
    dnxf_state_cable_swap_is_master_cbs is_master;
} dnxf_state_cable_swap_cbs;

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
    dnxf_state_lane_map_db_serdes_map_map_size_set_cb set;
    dnxf_state_lane_map_db_serdes_map_map_size_get_cb get;
} dnxf_state_lane_map_db_serdes_map_map_size_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_set_cb set;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_get_cb get;
} dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_set_cb set;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_get_cb get;
} dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_lane2serdes_set_cb set;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_get_cb get;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_alloc_cb alloc;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_cbs rx_id;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_cbs tx_id;
} dnxf_state_lane_map_db_serdes_map_lane2serdes_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_set_cb set;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_get_cb get;
} dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_set_cb set;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_get_cb get;
} dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_serdes2lane_set_cb set;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_get_cb get;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_alloc_cb alloc;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_cbs rx_id;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_cbs tx_id;
} dnxf_state_lane_map_db_serdes_map_serdes2lane_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_map_size_cbs map_size;
    dnxf_state_lane_map_db_serdes_map_lane2serdes_cbs lane2serdes;
    dnxf_state_lane_map_db_serdes_map_serdes2lane_cbs serdes2lane;
} dnxf_state_lane_map_db_serdes_map_cbs;

typedef struct {
    dnxf_state_lane_map_db_link_to_fmac_lane_map_set_cb set;
    dnxf_state_lane_map_db_link_to_fmac_lane_map_get_cb get;
    dnxf_state_lane_map_db_link_to_fmac_lane_map_alloc_cb alloc;
} dnxf_state_lane_map_db_link_to_fmac_lane_map_cbs;

typedef struct {
    dnxf_state_lane_map_db_serdes_map_cbs serdes_map;
    dnxf_state_lane_map_db_link_to_fmac_lane_map_cbs link_to_fmac_lane_map;
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
    dnxf_state_synce_master_set1_synce_enabled_set_cb set;
    dnxf_state_synce_master_set1_synce_enabled_get_cb get;
} dnxf_state_synce_master_set1_synce_enabled_cbs;

typedef struct {
    dnxf_state_synce_slave_set1_synce_enabled_set_cb set;
    dnxf_state_synce_slave_set1_synce_enabled_get_cb get;
} dnxf_state_synce_slave_set1_synce_enabled_cbs;

typedef struct {
    dnxf_state_synce_master_synce_enabled_cbs master_synce_enabled;
    dnxf_state_synce_slave_synce_enabled_cbs slave_synce_enabled;
    dnxf_state_synce_master_set1_synce_enabled_cbs master_set1_synce_enabled;
    dnxf_state_synce_slave_set1_synce_enabled_cbs slave_set1_synce_enabled;
} dnxf_state_synce_cbs;

typedef struct {
    dnxf_state_system_upgrade_state_0_set_cb set;
    dnxf_state_system_upgrade_state_0_get_cb get;
} dnxf_state_system_upgrade_state_0_cbs;

typedef struct {
    dnxf_state_system_upgrade_state_1_set_cb set;
    dnxf_state_system_upgrade_state_1_get_cb get;
} dnxf_state_system_upgrade_state_1_cbs;

typedef struct {
    dnxf_state_system_upgrade_state_2_set_cb set;
    dnxf_state_system_upgrade_state_2_get_cb get;
} dnxf_state_system_upgrade_state_2_cbs;

typedef struct {
    dnxf_state_system_upgrade_state_0_cbs state_0;
    dnxf_state_system_upgrade_state_1_cbs state_1;
    dnxf_state_system_upgrade_state_2_cbs state_2;
} dnxf_state_system_upgrade_cbs;

typedef struct {
    dnxf_state_hard_reset_callback_set_cb set;
    dnxf_state_hard_reset_callback_get_cb get;
} dnxf_state_hard_reset_callback_cbs;

typedef struct {
    dnxf_state_hard_reset_userdata_set_cb set;
    dnxf_state_hard_reset_userdata_get_cb get;
} dnxf_state_hard_reset_userdata_cbs;

typedef struct {
    dnxf_state_hard_reset_callback_cbs callback;
    dnxf_state_hard_reset_userdata_cbs userdata;
} dnxf_state_hard_reset_cbs;

typedef struct {
    dnxf_state_mib_counter_pbmp_set_cb set;
    dnxf_state_mib_counter_pbmp_get_cb get;
    dnxf_state_mib_counter_pbmp_pbmp_neq_cb pbmp_neq;
    dnxf_state_mib_counter_pbmp_pbmp_eq_cb pbmp_eq;
    dnxf_state_mib_counter_pbmp_pbmp_member_cb pbmp_member;
    dnxf_state_mib_counter_pbmp_pbmp_not_null_cb pbmp_not_null;
    dnxf_state_mib_counter_pbmp_pbmp_is_null_cb pbmp_is_null;
    dnxf_state_mib_counter_pbmp_pbmp_count_cb pbmp_count;
    dnxf_state_mib_counter_pbmp_pbmp_xor_cb pbmp_xor;
    dnxf_state_mib_counter_pbmp_pbmp_remove_cb pbmp_remove;
    dnxf_state_mib_counter_pbmp_pbmp_assign_cb pbmp_assign;
    dnxf_state_mib_counter_pbmp_pbmp_get_cb pbmp_get;
    dnxf_state_mib_counter_pbmp_pbmp_and_cb pbmp_and;
    dnxf_state_mib_counter_pbmp_pbmp_negate_cb pbmp_negate;
    dnxf_state_mib_counter_pbmp_pbmp_or_cb pbmp_or;
    dnxf_state_mib_counter_pbmp_pbmp_clear_cb pbmp_clear;
    dnxf_state_mib_counter_pbmp_pbmp_port_add_cb pbmp_port_add;
    dnxf_state_mib_counter_pbmp_pbmp_port_flip_cb pbmp_port_flip;
    dnxf_state_mib_counter_pbmp_pbmp_port_remove_cb pbmp_port_remove;
    dnxf_state_mib_counter_pbmp_pbmp_port_set_cb pbmp_port_set;
    dnxf_state_mib_counter_pbmp_pbmp_ports_range_add_cb pbmp_ports_range_add;
    dnxf_state_mib_counter_pbmp_pbmp_fmt_cb pbmp_fmt;
} dnxf_state_mib_counter_pbmp_cbs;

typedef struct {
    dnxf_state_mib_interval_set_cb set;
    dnxf_state_mib_interval_get_cb get;
} dnxf_state_mib_interval_cbs;

typedef struct {
    dnxf_state_mib_counter_pbmp_cbs counter_pbmp;
    dnxf_state_mib_interval_cbs interval;
} dnxf_state_mib_cbs;

typedef struct {
    dnxf_state_is_init_cb is_init;
    dnxf_state_init_cb init;
    dnxf_state_info_cbs info;
    dnxf_state_modid_cbs modid;
    dnxf_state_mc_cbs mc;
    dnxf_state_lb_cbs lb;
    dnxf_state_port_cbs port;
    dnxf_state_cable_swap_cbs cable_swap;
    dnxf_state_intr_cbs intr;
    dnxf_state_lane_map_db_cbs lane_map_db;
    dnxf_state_synce_cbs synce;
    dnxf_state_system_upgrade_cbs system_upgrade;
    dnxf_state_hard_reset_cbs hard_reset;
    dnxf_state_mib_cbs mib;
} dnxf_state_cbs;





extern dnxf_state_cbs dnxf_state;

#endif 
