/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.h
 * Purpose:     Multiplexing of the bcm layer - For fabric element functions
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * This code allows to use the same MBCM_DNXF_DRIVER_CALL API independently of the chip type
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_DNXF_CMN_MCBM_H
#define _SOC_DNXF_CMN_MCBM_H

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/chip.h>
#include <soc/error.h>

#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/vsc256_fabric_cell.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/dnxc_port.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/dnxf_fifo_dma.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_multicast.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/dnxf/cmn/dnxf_fabric_flow_control.h>
#include <soc/dnxf/cmn/dnxf_cgm.h>


typedef enum soc_dnxf_chip_family_e {
    BCM_FAMILY_RAMON
} soc_dnxf_chip_family_t;

#define BCM_DNXF_CHIP_FAMILY(unit)       (mbcm_dnxf_family[unit])



typedef int (*mbcm_dnxf_tbl_is_dynamic_f) (int unit, soc_mem_t mem);
typedef int (*mbcm_dnxf_ser_init_f) (int unit);
typedef int (*mbcm_dnxf_reset_device_f) (int unit);
typedef int (*mbcm_dnxf_drv_soft_init_f) (int unit, uint32 soft_reset_mode_flags);
typedef int (*mbcm_dnxf_drv_blocks_reset_f) (int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
typedef int (*mbcm_dnxf_drv_reg_access_only_reset_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_sr_cell_send_f) (int unit, const dnxc_vsc256_sr_cell_t* cell);
typedef shr_error_e (*mbcm_dnxf_cell_filter_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_cell_filter_set_f) (int unit, uint32 flags, uint32 array_size, soc_dnxc_filter_type_t* filter_type_arr, uint32* filter_type_val);
typedef shr_error_e (*mbcm_dnxf_cell_filter_clear_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_cell_filter_count_get_f) (int unit, int *count);
typedef shr_error_e (*mbcm_dnxf_control_cell_filter_set_f) (int unit, uint32 flags, soc_dnxc_control_cell_types_t cell_type, uint32 array_size, soc_dnxc_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val);
typedef shr_error_e (*mbcm_dnxf_control_cell_filter_clear_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_control_cell_filter_receive_f) (int unit,  soc_dnxc_captured_control_cell_t* data_out);
typedef int (*mbcm_dnxf_cell_snake_test_prepare_f) (int unit, uint32 flags);
typedef int (*mbcm_dnxf_cell_snake_test_run_f) (int unit, uint32 flags, soc_dnxf_fabric_cell_snake_test_results_t* results);
typedef shr_error_e (*mbcm_dnxf_diag_fabric_cell_snake_test_interrupts_name_get_f) (int unit, const soc_dnxf_diag_flag_str_t **interrupts_names);
typedef shr_error_e (*mbcm_dnxf_diag_cell_pipe_counter_get_f) (int unit, int pipe, uint64 *counter);
typedef shr_error_e (*mbcm_dnxf_diag_mesh_topology_get_f) (int unit, soc_dnxc_fabric_mesh_topology_diag_t *mesh_topology_diag);
typedef int (*mbcm_dnxf_drv_mbist_f) (int unit, int skip_errors);
typedef int (*mbcm_dnxf_drv_link_to_block_mapping_f) (int unit, int link, int* block_id,int* inner_link, int block_type);
typedef int (*mbcm_dnxf_drv_block_pbmp_get_f) (int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);
typedef shr_error_e (*mbcm_dnxf_fabric_cell_get_f) (int unit, soc_dnxc_fabric_cell_entry_t entry);
typedef shr_error_e (*mbcm_dnxf_fabric_cell_parse_table_get_f) (int unit, soc_dnxc_fabric_cell_entry_t entry, soc_dnxc_fabric_cell_type_t *cell_type, soc_dnxc_fabric_cell_parse_table_t *parse_table);
typedef shr_error_e (*mbcm_dnxf_cgm_threshold_to_reg_table_get_f) (int unit,const soc_dnxf_cgm_threshold_to_reg_binding_t **reg_binding_table);
typedef shr_error_e (*mbcm_dnxf_fabric_link_profile_set_f) (int unit, int profile_id,uint32 flags, uint32 links_count, bcm_port_t* links);
typedef shr_error_e (*mbcm_dnxf_fabric_link_profile_get_f) (int unit, int profile_id,uint32 flags, uint32 links_count_max, uint32 *links_count, bcm_port_t *links);
typedef shr_error_e (*mbcm_dnxf_fabric_rci_resolution_set_f) (int unit, soc_dnxf_fabric_rci_resolution_key_t *key, soc_dnxf_fabric_rci_resolution_config_t *config);
typedef shr_error_e (*mbcm_dnxf_fabric_rci_resolution_get_f) (int unit, soc_dnxf_fabric_rci_resolution_key_t *key, soc_dnxf_fabric_rci_resolution_config_t *config);
typedef shr_error_e (*mbcm_dnxf_fabric_links_validate_link_f) (int unit, soc_port_t link);
typedef shr_error_e (*mbcm_dnxf_fabric_links_isolate_set_f) (int unit, soc_port_t link, soc_dnxc_isolation_status_t val);
typedef shr_error_e (*mbcm_dnxf_fabric_links_isolate_get_f) (int unit, soc_port_t link, soc_dnxc_isolation_status_t* val);
typedef shr_error_e (*mbcm_dnxf_fabric_links_flow_status_control_cell_format_set_f) (int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t val);
typedef shr_error_e (*mbcm_dnxf_fabric_links_flow_status_control_cell_format_get_f) (int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t* val);
typedef shr_error_e (*mbcm_dnxf_fabric_links_llf_control_source_set_f) (int unit, soc_port_t link, soc_dnxc_fabric_pipe_t val);
typedef shr_error_e (*mbcm_dnxf_fabric_links_llf_control_source_get_f) (int unit, soc_port_t link, soc_dnxc_fabric_pipe_t* val); 
typedef shr_error_e (*mbcm_dnxf_fabric_links_aldwp_config_f) (int unit, soc_port_t port);
typedef shr_error_e (*mbcm_dnxf_fabric_links_aldwp_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_fabric_links_pcp_enable_set_f) (int unit, soc_port_t link, int enable);
typedef shr_error_e (*mbcm_dnxf_fabric_links_pcp_enable_get_f) (int unit, soc_port_t link, int *enable);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_set_f) (int unit, soc_dnxf_fabric_priority_t prio);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_get_f) (int unit, soc_dnxf_fabric_priority_t* prio);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_prio_threshold_validate_f) (int unit, bcm_fabric_control_t type, int arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_prio_threshold_set_f) (int unit, bcm_fabric_control_t type, int arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_prio_threshold_get_f) (int unit, bcm_fabric_control_t type, int* arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_priority_drop_enable_set_f) (int unit, int arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_low_priority_drop_enable_get_f) (int unit, int* arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_priority_range_validate_f) (int unit, bcm_fabric_control_t type, int arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_priority_range_set_f) (int unit, bcm_fabric_control_t type, int arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_priority_range_get_f) (int unit, bcm_fabric_control_t type, int* arg);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_multi_set_f) (int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array, int *local_route_enable_array);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_multi_get_f) (int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array, int *local_route_enable_array);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_multi_write_range_f) (int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_multi_read_range_f) (int unit, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_local_route_default_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_local_route_bit_get_f) (int unit, uint32 *mc_local_route_bit);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_direct_offset_set_f) (int unit, int fap_modid_offset);
typedef shr_error_e (*mbcm_dnxf_fabric_multicast_direct_offset_get_f) (int unit, int* fap_modid_offset);
typedef shr_error_e (*mbcm_dnxf_fabric_link_repeater_enable_set_f) (int unit, soc_port_t port, int enable);
typedef shr_error_e (*mbcm_dnxf_fabric_link_repeater_enable_get_f) (int unit, soc_port_t port, int *enable);
typedef shr_error_e (*mbcm_dnxf_fabric_link_retimer_calibrate_f) (int unit, soc_port_t link_a, soc_port_t link_b);
typedef shr_error_e (*mbcm_dnxf_fabric_link_retimer_connect_set_f) (int unit, soc_port_t link_a, soc_port_t link_b);
typedef shr_error_e (*mbcm_dnxf_fabric_link_retimer_connect_get_f) (int unit, soc_port_t link_a, soc_port_t *link_b);
typedef shr_error_e (*mbcm_dnxf_fabric_link_status_get_f) (int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count);
typedef shr_error_e (*mbcm_dnxf_fabric_reachability_status_get_f) (int unit, int moduleid, int links_max, uint32 *links_array, int *links_count);
typedef shr_error_e (*mbcm_dnxf_fabric_link_connectivity_status_get_f) (int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner);
typedef shr_error_e (*mbcm_dnxf_fabric_stat_get_f) (int unit, bcm_fabric_stat_index_t index, bcm_fabric_stat_t stat, uint64 *value);
typedef shr_error_e (*mbcm_dnxf_fabric_links_pipe_map_set_f) (int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t pipe_map);
typedef shr_error_e (*mbcm_dnxf_fabric_links_pipe_map_get_f) (int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t *pipe_map);
typedef shr_error_e (*mbcm_dnxf_fabric_links_link_integrity_down_set_f) (int unit, soc_port_t port, int force_down);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_isolate_set_f) (int unit, soc_dnxc_isolation_status_t val);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_isolate_get_f) (int unit, soc_dnxc_isolation_status_t* val);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_rmgr_set_f) (int unit, int enable);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_local_route_control_cells_enable_set_f) (int unit, int enable);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_lb_set_f) (int unit, bcm_module_t destination_local_id, int links_count, soc_port_t *links_array);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_lb_get_f) (int unit, bcm_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_modid_group_map_set_f) (int unit, soc_module_t modid, int group);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_mc_sw_control_set_f) (int unit, int group, int enable);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_mc_sw_control_get_f) (int unit, int group, int *enable);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_mc_set_f) (int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array);
typedef shr_error_e (*mbcm_dnxf_fabric_topology_mc_get_f) (int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
typedef shr_error_e (*mbcm_dnxf_fifo_dma_channel_init_f) (int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
typedef shr_error_e (*mbcm_dnxf_fifo_dma_channel_deinit_f) (int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
typedef shr_error_e (*mbcm_dnxf_fifo_dma_channel_read_entries_f) (int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
typedef shr_error_e (*mbcm_dnxf_fifo_dma_fabric_cell_validate_f) (int unit);
typedef int (*mbcm_dnxf_nof_interrupts_f) (int unit, int* nof_interrupts);
typedef int (*mbcm_dnxf_nof_block_instances_f) (int unit, soc_block_types_t block_types, int *nof_block_instances);
typedef shr_error_e (*mbcm_dnxf_multicast_egress_add_f) (int unit, bcm_multicast_t group, soc_gport_t port);
typedef shr_error_e (*mbcm_dnxf_multicast_egress_delete_f) (int unit, bcm_multicast_t group, soc_gport_t port);
typedef shr_error_e (*mbcm_dnxf_multicast_egress_delete_all_f) (int unit, bcm_multicast_t group);
typedef shr_error_e (*mbcm_dnxf_multicast_egress_get_f) (int unit, bcm_multicast_t group, int port_max, soc_gport_t *port_array, int *port_count);
typedef shr_error_e (*mbcm_dnxf_multicast_egress_set_f) (int unit, bcm_multicast_t group, int port_count, soc_gport_t *port_array);
typedef shr_error_e (*mbcm_dnxf_multicast_mode_get_f) (int unit, soc_dnxf_multicast_table_mode_t* multicast_mode); 
typedef shr_error_e (*mbcm_dnxf_multicast_table_size_get_f) (int unit, uint32* mc_table_size);
typedef shr_error_e (*mbcm_dnxf_multicast_table_entry_size_get_f) (int unit, uint32* entry_size);
typedef shr_error_e (*mbcm_dnxf_port_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_port_deinit_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_port_detach_f) (int unit, soc_port_t port);
typedef shr_error_e (*mbcm_dnxf_port_soc_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_port_probe_f) (int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
typedef shr_error_e (*mbcm_dnxf_lane_map_db_map_set_f) (int unit, int nof_lanes, soc_dnxc_lane_map_db_map_t* lane2serdes, int is_init_sequence);
typedef shr_error_e (*mbcm_dnxf_lane_map_db_map_get_f) (int unit, int nof_lanes, soc_dnxc_lane_map_db_map_t* lane2serdes);
typedef shr_error_e (*mbcm_dnxf_port_control_power_set_f) (int unit, soc_port_t port, uint32 flags, soc_dnxc_port_power_t power);
typedef shr_error_e (*mbcm_dnxf_port_control_power_get_f) (int unit, soc_port_t port, soc_dnxc_port_power_t* power);
typedef shr_error_e (*mbcm_dnxf_port_control_low_latency_set_f) (int unit,soc_port_t port,int value);
typedef shr_error_e (*mbcm_dnxf_port_control_low_latency_get_f) (int unit,soc_port_t port,int* value);
typedef shr_error_e (*mbcm_dnxf_port_control_fec_error_detect_set_f) (int unit,soc_port_t port,int value);
typedef shr_error_e (*mbcm_dnxf_port_control_fec_error_detect_get_f) (int unit,soc_port_t port,int* value);
typedef shr_error_e (*mbcm_dnxf_port_control_llfc_after_fec_enable_set_f) (int unit,soc_port_t port,int value);
typedef shr_error_e (*mbcm_dnxf_port_control_llfc_after_fec_enable_get_f) (int unit,soc_port_t port,int* value);
typedef shr_error_e (*mbcm_dnxf_port_phy_cl72_set_f) (int unit, soc_port_t port, int enable);
typedef shr_error_e (*mbcm_dnxf_port_phy_cl72_get_f) (int unit, soc_port_t port, int *enable);
typedef shr_error_e (*mbcm_dnxf_port_phy_control_set_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
typedef shr_error_e (*mbcm_dnxf_port_phy_control_get_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value);
typedef shr_error_e (*mbcm_dnxf_port_loopback_set_f) (int unit, soc_port_t port, portmod_loopback_mode_t loopback);
typedef shr_error_e (*mbcm_dnxf_port_loopback_get_f) (int unit, soc_port_t port, portmod_loopback_mode_t* loopback);
typedef shr_error_e (*mbcm_dnxf_port_fault_get_f) (int unit, soc_port_t port, uint32* flags);
typedef shr_error_e (*mbcm_dnxf_port_speed_max_f) (int unit, soc_port_t port, int *speed);
typedef shr_error_e (*mbcm_dnxf_port_serdes_power_disable_f) (int unit, soc_pbmp_t disable_pbmp);
typedef shr_error_e (*mbcm_dnxf_port_link_status_get_f) (int unit, soc_port_t port, int *up);
typedef shr_error_e (*mbcm_dnxf_port_bucket_fill_rate_validate_f) (int unit, uint32 bucket_fill_rate);
typedef shr_error_e (*mbcm_dnxf_port_prbs_tx_enable_set_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_tx_enable_get_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_rx_enable_set_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_rx_enable_get_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_rx_status_get_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_polynomial_set_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_polynomial_get_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_tx_invert_data_set_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
typedef shr_error_e (*mbcm_dnxf_port_prbs_tx_invert_data_get_f) (int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value);
typedef shr_error_e (*mbcm_dnxf_port_pump_enable_set_f) (int unit, soc_port_t port, int enable);
typedef shr_error_e (*mbcm_dnxf_port_rate_egress_ppt_set_f) (int unit, soc_port_t port, uint32 burst, uint32 nof_tiks);
typedef shr_error_e (*mbcm_dnxf_port_rate_egress_ppt_get_f) (int unit, soc_port_t port, uint32 *burst, uint32 *nof_tiks);
typedef shr_error_e (*mbcm_dnxf_port_burst_control_set_f)(int unit, soc_port_t port, portmod_loopback_mode_t loopback);
typedef shr_error_e (*mbcm_dnxf_port_extract_cig_from_llfc_enable_set_f) (int unit, soc_port_t port, int value);
typedef shr_error_e (*mbcm_dnxf_port_extract_cig_from_llfc_enable_get_f) (int unit, soc_port_t port, int *value);
typedef shr_error_e (*mbcm_dnxf_port_phy_get_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
typedef shr_error_e (*mbcm_dnxf_port_phy_set_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
typedef shr_error_e (*mbcm_dnxf_port_phy_modify_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
typedef shr_error_e (*mbcm_dnxf_port_dynamic_port_update_f) (int unit, int port, int add_remove, soc_dnxf_port_update_type_t type);
typedef shr_error_e (*mbcm_dnxf_port_enable_set_f) (int unit, int port, int enable);
typedef shr_error_e (*mbcm_dnxf_port_enable_get_f) (int unit, int port, int *enable);
typedef shr_error_e (*mbcm_dnxf_port_sync_e_link_set_f) (int unit, int is_master, int arg);
typedef shr_error_e (*mbcm_dnxf_port_sync_e_divider_set_f) (int unit, int is_master, int divider);
typedef shr_error_e (*mbcm_dnxf_port_sync_e_link_get_f) (int unit, int is_master, int *arg);
typedef shr_error_e (*mbcm_dnxf_port_sync_e_divider_get_f) (int unit, int is_master, int *arg);
typedef shr_error_e (*mbcm_dnxf_set_mesh_topology_config_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_stk_modid_set_f) (int unit, int fe_id);
typedef shr_error_e (*mbcm_dnxf_stk_modid_get_f) (int unit, int* fe_id);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_all_reachable_verify_f) (int unit, bcm_module_t max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_all_reachable_set_f) (int unit, bcm_module_t max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_all_reachable_get_f) (int unit, bcm_module_t *max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_fap_verify_f) (int unit, bcm_module_t max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_fap_set_f) (int unit, bcm_module_t max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_max_fap_get_f) (int unit, bcm_module_t *max_module);
typedef shr_error_e (*mbcm_dnxf_stk_module_all_reachable_ignore_id_set_f) (int unit, bcm_module_t module, int arg);
typedef shr_error_e (*mbcm_dnxf_stk_module_all_reachable_ignore_id_get_f) (int unit, bcm_module_t module, int *arg);
typedef shr_error_e (*mbcm_dnxf_stk_valid_module_id_verify_f) (int unit, bcm_module_t module, int verify_divisble_by_32);
typedef shr_error_e (*mbcm_dnxf_stat_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_stat_is_supported_type_f) (int unit, soc_port_t port, bcm_stat_val_t type);
typedef shr_error_e (*mbcm_dnxf_stat_counter_length_get_f) (int unit, int counter_id, int *length);
typedef shr_error_e (*mbcm_dnxf_stat_get_f) (int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size);
typedef shr_error_e (*mbcm_dnxf_mapping_stat_get_f) (int unit,soc_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
typedef shr_error_e (*mbcm_dnxf_counters_get_info_f) (int unit,soc_dnxf_counters_info_t* fe_counters_info);
typedef shr_error_e (*mbcm_dnxf_queues_get_info_f) (int unit,soc_dnxf_queues_info_t* fe_queues_info);
typedef shr_error_e (*mbcm_dnxf_fabric_link_device_mode_get_f) (int unit,soc_port_t port, int is_rx, soc_dnxf_fabric_link_device_mode_t *device_mode);
typedef shr_error_e (*mbcm_dnxf_controlled_counter_set_f) (int unit);
typedef int (*mbcm_dnxf_linkctrl_init_f) (int unit);
typedef int (*mbcm_dnxf_interrupts_init_f) (int unit);
typedef int (*mbcm_dnxf_interrupts_deinit_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_interrupt_all_enable_set_f) (int unit, int enable);
typedef shr_error_e (*mbcm_dnxf_interrupt_all_enable_get_f) (int unit, int *enable);
typedef int (*mbcm_dnxf_temperature_monitor_regs_get_f) (int unit, const soc_reg_t **temp_reg_ptr, const soc_field_t **currr_temp_field_ptr, const soc_field_t **peak_temp_field_ptr);
typedef shr_error_e (*mbcm_dnxf_cosq_pipe_weight_set_f) (int unit, soc_dnxf_cosq_pipe_type_t pipe_type, int pipe, int port, soc_dnxf_cosq_weight_mode_t mode, int weight);
typedef shr_error_e (*mbcm_dnxf_cosq_pipe_weight_get_f) (int unit, soc_dnxf_cosq_pipe_type_t pipe_type, int pipe, int port, soc_dnxf_cosq_weight_mode_t mode, int *weight);
typedef shr_error_e (*mbcm_dnxf_rx_cpu_address_modid_set_f) (int unit, int num_of_cpu_addresses, int *cpu_addresses);
typedef shr_error_e (*mbcm_dnxf_rx_cpu_address_modid_init_f) (int unit);
typedef shr_error_e (*mbcm_dnxf_drv_block_valid_get_f) (int unit, int blktype, int id, char *valid);
typedef shr_error_e (*mbcm_dnxf_port_stat_gtimer_config_set_f) (int unit, soc_port_t port, soc_dnxc_port_stat_gtimer_config_t *gtimer_config);
typedef shr_error_e (*mbcm_dnxf_port_stat_gtimer_config_get_f) (int unit, soc_port_t port, soc_dnxc_port_stat_gtimer_config_t *gtimer_config);


typedef struct mbcm_dnxf_functions_s {
     
    mbcm_dnxf_tbl_is_dynamic_f                                           mbcm_dnxf_tbl_is_dynamic;
    mbcm_dnxf_ser_init_f                                                 mbcm_dnxf_ser_init;
    mbcm_dnxf_reset_device_f                                             mbcm_dnxf_reset_device;
    mbcm_dnxf_drv_soft_init_f                                            mbcm_dnxf_drv_soft_init;
    mbcm_dnxf_drv_blocks_reset_f                                         mbcm_dnxf_drv_blocks_reset;
    mbcm_dnxf_drv_reg_access_only_reset_f                                mbcm_dnxf_drv_reg_access_only_reset;
    mbcm_dnxf_sr_cell_send_f                                             mbcm_dnxf_sr_cell_send;
    mbcm_dnxf_cell_filter_init_f                                         mbcm_dnxf_cell_filter_init;
    mbcm_dnxf_cell_filter_set_f                                          mbcm_dnxf_cell_filter_set;
    mbcm_dnxf_cell_filter_clear_f                                        mbcm_dnxf_cell_filter_clear;
    mbcm_dnxf_cell_filter_count_get_f                                    mbcm_dnxf_cell_filter_count_get;
    mbcm_dnxf_control_cell_filter_set_f                                  mbcm_dnxf_control_cell_filter_set;
    mbcm_dnxf_control_cell_filter_clear_f                                mbcm_dnxf_control_cell_filter_clear;
    mbcm_dnxf_control_cell_filter_receive_f                              mbcm_dnxf_control_cell_filter_receive;
    mbcm_dnxf_cell_snake_test_prepare_f                                  mbcm_dnxf_cell_snake_test_prepare;
    mbcm_dnxf_cell_snake_test_run_f                                      mbcm_dnxf_cell_snake_test_run;
    mbcm_dnxf_diag_fabric_cell_snake_test_interrupts_name_get_f          mbcm_dnxf_diag_fabric_cell_snake_test_interrupts_name_get;
    mbcm_dnxf_diag_cell_pipe_counter_get_f                               mbcm_dnxf_diag_cell_pipe_counter_get;
    mbcm_dnxf_diag_mesh_topology_get_f                                   mbcm_dnxf_diag_mesh_topology_get;
    mbcm_dnxf_drv_mbist_f                                                mbcm_dnxf_drv_mbist;
    mbcm_dnxf_drv_link_to_block_mapping_f                                mbcm_dnxf_drv_link_to_block_mapping;
    mbcm_dnxf_drv_block_pbmp_get_f                                       mbcm_dnxf_drv_block_pbmp_get;
    mbcm_dnxf_fabric_cell_get_f                                          mbcm_dnxf_fabric_cell_get;
    mbcm_dnxf_fabric_cell_parse_table_get_f                              mbcm_dnxf_fabric_cell_parse_table_get;
    mbcm_dnxf_cgm_threshold_to_reg_table_get_f                           mbcm_dnxf_cgm_threshold_to_reg_table_get;
    mbcm_dnxf_fabric_link_profile_set_f                                  mbcm_dnxf_fabric_link_profile_set;
    mbcm_dnxf_fabric_link_profile_get_f                                  mbcm_dnxf_fabric_link_profile_get;
    mbcm_dnxf_fabric_rci_resolution_set_f                                mbcm_dnxf_fabric_rci_resolution_set;
    mbcm_dnxf_fabric_rci_resolution_get_f                                mbcm_dnxf_fabric_rci_resolution_get;
    mbcm_dnxf_fabric_links_validate_link_f                               mbcm_dnxf_fabric_links_validate_link;
    mbcm_dnxf_fabric_links_isolate_set_f                                 mbcm_dnxf_fabric_links_isolate_set;
    mbcm_dnxf_fabric_links_isolate_get_f                                 mbcm_dnxf_fabric_links_isolate_get;
    mbcm_dnxf_fabric_links_flow_status_control_cell_format_set_f         mbcm_dnxf_fabric_links_flow_status_control_cell_format_set;
    mbcm_dnxf_fabric_links_flow_status_control_cell_format_get_f         mbcm_dnxf_fabric_links_flow_status_control_cell_format_get;
    mbcm_dnxf_fabric_links_llf_control_source_set_f                      mbcm_dnxf_fabric_links_llf_control_source_set;
    mbcm_dnxf_fabric_links_llf_control_source_get_f                      mbcm_dnxf_fabric_links_llf_control_source_get;
    mbcm_dnxf_fabric_links_aldwp_config_f                                mbcm_dnxf_fabric_links_aldwp_config;
    mbcm_dnxf_fabric_links_aldwp_init_f                                  mbcm_dnxf_fabric_links_aldwp_init;
    mbcm_dnxf_fabric_links_pcp_enable_set_f                              mbcm_dnxf_fabric_links_pcp_enable_set;
    mbcm_dnxf_fabric_links_pcp_enable_get_f                              mbcm_dnxf_fabric_links_pcp_enable_get;
    mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_set_f       mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_set;
    mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_get_f       mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_get;
    mbcm_dnxf_fabric_multicast_low_prio_threshold_validate_f             mbcm_dnxf_fabric_multicast_low_prio_threshold_validate;
    mbcm_dnxf_fabric_multicast_low_prio_threshold_set_f                  mbcm_dnxf_fabric_multicast_low_prio_threshold_set;
    mbcm_dnxf_fabric_multicast_low_prio_threshold_get_f                  mbcm_dnxf_fabric_multicast_low_prio_threshold_get;
    mbcm_dnxf_fabric_multicast_low_priority_drop_enable_set_f            mbcm_dnxf_fabric_multicast_low_priority_drop_enable_set;
    mbcm_dnxf_fabric_multicast_low_priority_drop_enable_get_f            mbcm_dnxf_fabric_multicast_low_priority_drop_enable_get;
    mbcm_dnxf_fabric_multicast_priority_range_validate_f                 mbcm_dnxf_fabric_multicast_priority_range_validate;
    mbcm_dnxf_fabric_multicast_priority_range_set_f                      mbcm_dnxf_fabric_multicast_priority_range_set;
    mbcm_dnxf_fabric_multicast_priority_range_get_f                      mbcm_dnxf_fabric_multicast_priority_range_get;
    mbcm_dnxf_fabric_multicast_multi_set_f                               mbcm_dnxf_fabric_multicast_multi_set;
    mbcm_dnxf_fabric_multicast_multi_get_f                               mbcm_dnxf_fabric_multicast_multi_get;
    mbcm_dnxf_fabric_multicast_multi_write_range_f                       mbcm_dnxf_fabric_multicast_multi_write_range;
    mbcm_dnxf_fabric_multicast_multi_read_range_f                        mbcm_dnxf_fabric_multicast_multi_read_range;
    mbcm_dnxf_fabric_multicast_local_route_default_init_f                mbcm_dnxf_fabric_multicast_local_route_default_init;
    mbcm_dnxf_fabric_multicast_local_route_bit_get_f                     mbcm_dnxf_fabric_multicast_local_route_bit_get;
    mbcm_dnxf_fabric_multicast_direct_offset_set_f                       mbcm_dnxf_fabric_multicast_direct_offset_set;
    mbcm_dnxf_fabric_multicast_direct_offset_get_f                       mbcm_dnxf_fabric_multicast_direct_offset_get;
    mbcm_dnxf_fabric_link_repeater_enable_set_f                          mbcm_dnxf_fabric_link_repeater_enable_set;
    mbcm_dnxf_fabric_link_repeater_enable_get_f                          mbcm_dnxf_fabric_link_repeater_enable_get;
    mbcm_dnxf_fabric_link_retimer_calibrate_f                            mbcm_dnxf_fabric_link_retimer_calibrate;
    mbcm_dnxf_fabric_link_retimer_connect_set_f                          mbcm_dnxf_fabric_link_retimer_connect_set;
    mbcm_dnxf_fabric_link_retimer_connect_get_f                          mbcm_dnxf_fabric_link_retimer_connect_get;
    mbcm_dnxf_fabric_link_status_get_f                                   mbcm_dnxf_fabric_link_status_get;
    mbcm_dnxf_fabric_reachability_status_get_f                           mbcm_dnxf_fabric_reachability_status_get;
    mbcm_dnxf_fabric_link_connectivity_status_get_f                      mbcm_dnxf_fabric_link_connectivity_status_get;
    mbcm_dnxf_fabric_stat_get_f                                          mbcm_dnxf_fabric_stat_get;
    mbcm_dnxf_fabric_links_pipe_map_set_f                                mbcm_dnxf_fabric_links_pipe_map_set;
    mbcm_dnxf_fabric_links_pipe_map_get_f                                mbcm_dnxf_fabric_links_pipe_map_get;
    mbcm_dnxf_fabric_links_link_integrity_down_set_f                     mbcm_dnxf_fabric_links_link_integrity_down_set;
    mbcm_dnxf_fabric_topology_isolate_set_f                              mbcm_dnxf_fabric_topology_isolate_set;
    mbcm_dnxf_fabric_topology_isolate_get_f                              mbcm_dnxf_fabric_topology_isolate_get;
    mbcm_dnxf_fabric_topology_rmgr_set_f                                 mbcm_dnxf_fabric_topology_rmgr_set;
    mbcm_dnxf_fabric_topology_local_route_control_cells_enable_set_f     mbcm_dnxf_fabric_topology_local_route_control_cells_enable_set;
    mbcm_dnxf_fabric_topology_lb_set_f                                   mbcm_dnxf_fabric_topology_lb_set;
    mbcm_dnxf_fabric_topology_lb_get_f                                   mbcm_dnxf_fabric_topology_lb_get;
    mbcm_dnxf_fabric_topology_modid_group_map_set_f                      mbcm_dnxf_fabric_topology_modid_group_map_set;
    mbcm_dnxf_fabric_topology_mc_sw_control_set_f                        mbcm_dnxf_fabric_topology_mc_sw_control_set;
    mbcm_dnxf_fabric_topology_mc_sw_control_get_f                        mbcm_dnxf_fabric_topology_mc_sw_control_get;
    mbcm_dnxf_fabric_topology_mc_set_f                                   mbcm_dnxf_fabric_topology_mc_set;
    mbcm_dnxf_fabric_topology_mc_get_f                                   mbcm_dnxf_fabric_topology_mc_get;
    mbcm_dnxf_fifo_dma_channel_init_f                                    mbcm_dnxf_fifo_dma_channel_init;
    mbcm_dnxf_fifo_dma_channel_deinit_f                                  mbcm_dnxf_fifo_dma_channel_deinit;
    mbcm_dnxf_fifo_dma_channel_read_entries_f                            mbcm_dnxf_fifo_dma_channel_read_entries;
    mbcm_dnxf_fifo_dma_fabric_cell_validate_f                            mbcm_dnxf_fifo_dma_fabric_cell_validate;
    mbcm_dnxf_nof_interrupts_f                                           mbcm_dnxf_nof_interrupts;
    mbcm_dnxf_nof_block_instances_f                                      mbcm_dnxf_nof_block_instance;
    mbcm_dnxf_multicast_egress_add_f                                     mbcm_dnxf_multicast_egress_add;
    mbcm_dnxf_multicast_egress_delete_f                                  mbcm_dnxf_multicast_egress_delete;
    mbcm_dnxf_multicast_egress_delete_all_f                              mbcm_dnxf_multicast_egress_delete_all;
    mbcm_dnxf_multicast_egress_get_f                                     mbcm_dnxf_multicast_egress_get;
    mbcm_dnxf_multicast_egress_set_f                                     mbcm_dnxf_multicast_egress_set;
    mbcm_dnxf_multicast_mode_get_f                                       mbcm_dnxf_multicast_mode_get;
    mbcm_dnxf_multicast_table_size_get_f                                 mbcm_dnxf_multicast_table_size_get;
    mbcm_dnxf_multicast_table_entry_size_get_f                           mbcm_dnxf_multicast_table_entry_size_get;
    mbcm_dnxf_port_soc_init_f                                            mbcm_dnxf_port_soc_init;
    mbcm_dnxf_port_init_f                                                mbcm_dnxf_port_init;
    mbcm_dnxf_port_deinit_f                                              mbcm_dnxf_port_deinit;
    mbcm_dnxf_port_detach_f                                              mbcm_dnxf_port_detach;  
    mbcm_dnxf_port_probe_f                                               mbcm_dnxf_port_probe;
    mbcm_dnxf_lane_map_db_map_set_f                                      mbcm_dnxf_lane_map_db_map_set;
    mbcm_dnxf_lane_map_db_map_get_f                                      mbcm_dnxf_lane_map_db_map_get;
    mbcm_dnxf_port_control_power_set_f                                   mbcm_dnxf_port_control_power_set;
    mbcm_dnxf_port_control_power_get_f                                   mbcm_dnxf_port_control_power_get;
    mbcm_dnxf_port_control_low_latency_set_f                             mbcm_dnxf_port_control_low_latency_set;
    mbcm_dnxf_port_control_low_latency_get_f                             mbcm_dnxf_port_control_low_latency_get;
    mbcm_dnxf_port_control_fec_error_detect_set_f                        mbcm_dnxf_port_control_fec_error_detect_set;
    mbcm_dnxf_port_control_fec_error_detect_get_f                        mbcm_dnxf_port_control_fec_error_detect_get;
    mbcm_dnxf_port_control_llfc_after_fec_enable_set_f                   mbcm_dnxf_port_control_llfc_after_fec_enable_set;
    mbcm_dnxf_port_control_llfc_after_fec_enable_get_f                   mbcm_dnxf_port_control_llfc_after_fec_enable_get;
    mbcm_dnxf_port_phy_cl72_set_f                                        mbcm_dnxf_port_phy_cl72_set;
    mbcm_dnxf_port_phy_cl72_get_f                                        mbcm_dnxf_port_phy_cl72_get;
    mbcm_dnxf_port_phy_control_set_f                                     mbcm_dnxf_port_phy_control_set;
    mbcm_dnxf_port_phy_control_get_f                                     mbcm_dnxf_port_phy_control_get;
    mbcm_dnxf_port_loopback_set_f                                        mbcm_dnxf_port_loopback_set;
    mbcm_dnxf_port_loopback_get_f                                        mbcm_dnxf_port_loopback_get;
    mbcm_dnxf_port_fault_get_f                                           mbcm_dnxf_port_fault_get;
    mbcm_dnxf_port_speed_max_f                                           mbcm_dnxf_port_speed_max;
    mbcm_dnxf_port_serdes_power_disable_f                                mbcm_dnxf_port_serdes_power_disable;
    mbcm_dnxf_port_link_status_get_f                                     mbcm_dnxf_port_link_status_get;
    mbcm_dnxf_port_bucket_fill_rate_validate_f                           mbcm_dnxf_bucket_fill_rate_validate;
    mbcm_dnxf_port_prbs_tx_enable_set_f                                  mbcm_dnxf_port_prbs_tx_enable_set;
    mbcm_dnxf_port_prbs_tx_enable_get_f                                  mbcm_dnxf_port_prbs_tx_enable_get;
    mbcm_dnxf_port_prbs_rx_enable_set_f                                  mbcm_dnxf_port_prbs_rx_enable_set;
    mbcm_dnxf_port_prbs_rx_enable_get_f                                  mbcm_dnxf_port_prbs_rx_enable_get;
    mbcm_dnxf_port_prbs_rx_status_get_f                                  mbcm_dnxf_port_prbs_rx_status_get;
    mbcm_dnxf_port_prbs_polynomial_set_f                                 mbcm_dnxf_port_prbs_polynomial_set;   
    mbcm_dnxf_port_prbs_polynomial_get_f                                 mbcm_dnxf_port_prbs_polynomial_get;
    mbcm_dnxf_port_prbs_tx_invert_data_set_f                             mbcm_dnxf_port_prbs_tx_invert_data_set;
    mbcm_dnxf_port_prbs_tx_invert_data_get_f                             mbcm_dnxf_port_prbs_tx_invert_data_get;
    mbcm_dnxf_port_pump_enable_set_f                                     mbcm_dnxf_port_pump_enable_set;
    mbcm_dnxf_port_rate_egress_ppt_set_f                                 mbcm_dnxf_port_rate_egress_ppt_set;
    mbcm_dnxf_port_rate_egress_ppt_get_f                                 mbcm_dnxf_port_rate_egress_ppt_get;
    mbcm_dnxf_port_burst_control_set_f                                   mbcm_dnxf_port_burst_control_set;
    mbcm_dnxf_port_extract_cig_from_llfc_enable_set_f                    mbcm_dnxf_port_extract_cig_from_llfc_enable_set; 
    mbcm_dnxf_port_extract_cig_from_llfc_enable_get_f                    mbcm_dnxf_port_extract_cig_from_llfc_enable_get;
    mbcm_dnxf_port_phy_get_f                                             mbcm_dnxf_port_phy_get;
    mbcm_dnxf_port_phy_set_f                                             mbcm_dnxf_port_phy_set;
    mbcm_dnxf_port_phy_modify_f                                          mbcm_dnxf_port_phy_modify;
    mbcm_dnxf_port_dynamic_port_update_f                                 mbcm_dnxf_port_dynamic_port_update;
    mbcm_dnxf_port_enable_set_f                                          mbcm_dnxf_port_enable_set;
    mbcm_dnxf_port_enable_get_f                                          mbcm_dnxf_port_enable_get;
    mbcm_dnxf_port_sync_e_link_set_f                                     mbcm_dnxf_port_sync_e_link_set;
    mbcm_dnxf_port_sync_e_divider_set_f                                  mbcm_dnxf_port_sync_e_divider_set;
    mbcm_dnxf_port_sync_e_link_get_f                                     mbcm_dnxf_port_sync_e_link_get;
    mbcm_dnxf_port_sync_e_divider_get_f                                  mbcm_dnxf_port_sync_e_divider_get;
    mbcm_dnxf_set_mesh_topology_config_f                                 mbcm_dnxf_set_mesh_topology_config;
    mbcm_dnxf_stk_modid_set_f                                            mbcm_dnxf_stk_modid_set;
    mbcm_dnxf_stk_modid_get_f                                            mbcm_dnxf_stk_modid_get;
    mbcm_dnxf_stk_module_max_all_reachable_verify_f                      mbcm_dnxf_stk_module_max_all_reachable_verify;
    mbcm_dnxf_stk_module_max_all_reachable_set_f                         mbcm_dnxf_stk_module_max_all_reachable_set;
    mbcm_dnxf_stk_module_max_all_reachable_get_f                         mbcm_dnxf_stk_module_max_all_reachable_get;
    mbcm_dnxf_stk_module_max_fap_verify_f                                mbcm_dnxf_stk_module_max_fap_verify;
    mbcm_dnxf_stk_module_max_fap_set_f                                   mbcm_dnxf_stk_module_max_fap_set;
    mbcm_dnxf_stk_module_max_fap_get_f                                   mbcm_dnxf_stk_module_max_fap_get;
    mbcm_dnxf_stk_module_all_reachable_ignore_id_set_f                   mbcm_dnxf_stk_module_all_reachable_ignore_id_set;
    mbcm_dnxf_stk_module_all_reachable_ignore_id_get_f                   mbcm_dnxf_stk_module_all_reachable_ignore_id_get;
    mbcm_dnxf_stk_valid_module_id_verify_f                               mbcm_dnxf_stk_valid_module_id_verify;
    mbcm_dnxf_stat_init_f                                                mbcm_dnxf_stat_init;
    mbcm_dnxf_stat_is_supported_type_f                                   mbcm_dnxf_stat_is_supported_type;
    mbcm_dnxf_stat_counter_length_get_f                                  mbcm_dnxf_stat_counter_length_get;
    mbcm_dnxf_stat_get_f                                                 mbcm_dnxf_stat_get;
    mbcm_dnxf_mapping_stat_get_f                                         mbcm_dnxf_mapping_stat_get;
    mbcm_dnxf_counters_get_info_f                                        mbcm_dnxf_counters_get_info;
    mbcm_dnxf_queues_get_info_f                                          mbcm_dnxf_queues_get_info;
    mbcm_dnxf_fabric_link_device_mode_get_f                              mbcm_dnxf_fabric_link_device_mode_get;
    mbcm_dnxf_controlled_counter_set_f                                   mbcm_dnxf_controlled_counter_set;
    mbcm_dnxf_linkctrl_init_f                                            mbcm_dnxf_linkctrl_init;
    mbcm_dnxf_interrupts_init_f                                          mbcm_dnxf_interrupts_init;
    mbcm_dnxf_interrupts_deinit_f                                        mbcm_dnxf_interrupts_deinit;
    mbcm_dnxf_interrupt_all_enable_set_f                                 mbcm_dnxf_interrupt_all_enable_set;
    mbcm_dnxf_interrupt_all_enable_get_f                                 mbcm_dnxf_interrupt_all_enable_get;
    mbcm_dnxf_temperature_monitor_regs_get_f                             mbcm_dnxf_temperature_monitor_regs_get;
    mbcm_dnxf_cosq_pipe_weight_set_f                                     mbcm_dnxf_cosq_pipe_weight_set;
    mbcm_dnxf_cosq_pipe_weight_get_f                                     mbcm_dnxf_cosq_pipe_weight_get;
    mbcm_dnxf_rx_cpu_address_modid_set_f                                 mbcm_dnxf_rx_cpu_address_modid_set;
    mbcm_dnxf_rx_cpu_address_modid_init_f                                mbcm_dnxf_rx_cpu_address_modid_init;
    mbcm_dnxf_drv_block_valid_get_f                                      mbcm_dnxf_drv_block_valid_get;
    mbcm_dnxf_port_stat_gtimer_config_set_f                              mbcm_dnxf_port_stat_gtimer_config_set;
    mbcm_dnxf_port_stat_gtimer_config_get_f                              mbcm_dnxf_port_stat_gtimer_config_get;
} mbcm_dnxf_functions_t; 


extern mbcm_dnxf_functions_t *mbcm_dnxf_driver[BCM_MAX_NUM_UNITS]; 


extern soc_dnxf_chip_family_t mbcm_dnxf_family[BCM_MAX_NUM_UNITS];


extern mbcm_dnxf_functions_t mbcm_ramon_driver;


extern int mbcm_dnxf_init(int unit);



#define MBCM_DNXF_DRIVER_CALL(unit, function, args) ( (mbcm_dnxf_driver[unit] && mbcm_dnxf_driver[unit]->function) ? \
  mbcm_dnxf_driver[unit]->function args : _SHR_E_UNAVAIL )
#define MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, function) ( (mbcm_dnxf_driver[unit] && mbcm_dnxf_driver[unit]->function) ? \
  mbcm_dnxf_driver[unit]->function(unit) : _SHR_E_UNAVAIL )
#define MBCM_DNXF_DRIVER_CALL_NO_ARGS_VOID(unit, function) ( (mbcm_dnxf_driver[unit] && mbcm_dnxf_driver[unit]->function) ? \
  mbcm_dnxf_driver[unit]->function(unit), _SHR_E_NONE : _SHR_E_UNAVAIL )
#define MBCM_DNXF_DRIVER_CALL_VOID(unit, function, args) ( (mbcm_dnxf_driver[unit] && mbcm_dnxf_driver[unit]->function) ? \
  mbcm_dnxf_driver[unit]->function args, _SHR_E_NONE : _SHR_E_UNAVAIL )

#if 0
#define MBCM_DNXF_DRIVER_CALL_WITHOUT_DEV_ID(unit, function, args...) ( (mbcm_dnxf_driver[unit] && mbcm_dnxf_driver[unit]->function) ? \
  mbcm_dnxf_driver[unit]->function(args) : _SHR_E_UNAVAIL )
#endif




#endif 
