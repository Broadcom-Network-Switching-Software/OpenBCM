

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_FABRIC_H_

#define _DNXF_DATA_INTERNAL_FABRIC_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_fabric_submodule_general,
    dnxf_data_fabric_submodule_pipes,
    dnxf_data_fabric_submodule_multicast,
    dnxf_data_fabric_submodule_fifos,
    dnxf_data_fabric_submodule_congestion,
    dnxf_data_fabric_submodule_cell,
    dnxf_data_fabric_submodule_topology,
    dnxf_data_fabric_submodule_gpd,
    dnxf_data_fabric_submodule_reachability,
    dnxf_data_fabric_submodule_hw_snake,

    
    _dnxf_data_fabric_submodule_nof
} dnxf_data_fabric_submodule_e;








int dnxf_data_fabric_general_feature_get(
    int unit,
    dnxf_data_fabric_general_feature_e feature);



typedef enum
{
    dnxf_data_fabric_general_define_max_nof_pipes,
    dnxf_data_fabric_general_define_device_mode,
    dnxf_data_fabric_general_define_local_routing_enable_uc,
    dnxf_data_fabric_general_define_local_routing_enable_mc,

    
    _dnxf_data_fabric_general_define_nof
} dnxf_data_fabric_general_define_e;



uint32 dnxf_data_fabric_general_max_nof_pipes_get(
    int unit);


uint32 dnxf_data_fabric_general_device_mode_get(
    int unit);


uint32 dnxf_data_fabric_general_local_routing_enable_uc_get(
    int unit);


uint32 dnxf_data_fabric_general_local_routing_enable_mc_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_general_table_nof
} dnxf_data_fabric_general_table_e;









int dnxf_data_fabric_pipes_feature_get(
    int unit,
    dnxf_data_fabric_pipes_feature_e feature);



typedef enum
{
    dnxf_data_fabric_pipes_define_max_nof_pipes,
    dnxf_data_fabric_pipes_define_nof_valid_mapping_options,
    dnxf_data_fabric_pipes_define_nof_pipes,
    dnxf_data_fabric_pipes_define_system_contains_multiple_pipe_device,
    dnxf_data_fabric_pipes_define_system_tdm_priority,

    
    _dnxf_data_fabric_pipes_define_nof
} dnxf_data_fabric_pipes_define_e;



uint32 dnxf_data_fabric_pipes_max_nof_pipes_get(
    int unit);


uint32 dnxf_data_fabric_pipes_nof_valid_mapping_options_get(
    int unit);


uint32 dnxf_data_fabric_pipes_nof_pipes_get(
    int unit);


uint32 dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_get(
    int unit);


uint32 dnxf_data_fabric_pipes_system_tdm_priority_get(
    int unit);



typedef enum
{
    dnxf_data_fabric_pipes_table_map,
    dnxf_data_fabric_pipes_table_valid_map_config,

    
    _dnxf_data_fabric_pipes_table_nof
} dnxf_data_fabric_pipes_table_e;



const dnxf_data_fabric_pipes_map_t * dnxf_data_fabric_pipes_map_get(
    int unit);


const dnxf_data_fabric_pipes_valid_map_config_t * dnxf_data_fabric_pipes_valid_map_config_get(
    int unit,
    int mapping_option_idx);



shr_error_e dnxf_data_fabric_pipes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_fabric_pipes_valid_map_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_fabric_pipes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_fabric_pipes_valid_map_config_info_get(
    int unit);






int dnxf_data_fabric_multicast_feature_get(
    int unit,
    dnxf_data_fabric_multicast_feature_e feature);



typedef enum
{
    dnxf_data_fabric_multicast_define_table_row_size_in_uint32,
    dnxf_data_fabric_multicast_define_id_range,
    dnxf_data_fabric_multicast_define_mode,
    dnxf_data_fabric_multicast_define_priority_map_enable,

    
    _dnxf_data_fabric_multicast_define_nof
} dnxf_data_fabric_multicast_define_e;



uint32 dnxf_data_fabric_multicast_table_row_size_in_uint32_get(
    int unit);


uint32 dnxf_data_fabric_multicast_id_range_get(
    int unit);


uint32 dnxf_data_fabric_multicast_mode_get(
    int unit);


uint32 dnxf_data_fabric_multicast_priority_map_enable_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_multicast_table_nof
} dnxf_data_fabric_multicast_table_e;









int dnxf_data_fabric_fifos_feature_get(
    int unit,
    dnxf_data_fabric_fifos_feature_e feature);



typedef enum
{
    dnxf_data_fabric_fifos_define_granularity,
    dnxf_data_fabric_fifos_define_min_depth,
    dnxf_data_fabric_fifos_define_max_unused_resources,
    dnxf_data_fabric_fifos_define_rx_resources,
    dnxf_data_fabric_fifos_define_mid_resources,
    dnxf_data_fabric_fifos_define_tx_resources,
    dnxf_data_fabric_fifos_define_rx_full_offset,
    dnxf_data_fabric_fifos_define_mid_full_offset,
    dnxf_data_fabric_fifos_define_tx_full_offset,

    
    _dnxf_data_fabric_fifos_define_nof
} dnxf_data_fabric_fifos_define_e;



uint32 dnxf_data_fabric_fifos_granularity_get(
    int unit);


uint32 dnxf_data_fabric_fifos_min_depth_get(
    int unit);


uint32 dnxf_data_fabric_fifos_max_unused_resources_get(
    int unit);


uint32 dnxf_data_fabric_fifos_rx_resources_get(
    int unit);


uint32 dnxf_data_fabric_fifos_mid_resources_get(
    int unit);


uint32 dnxf_data_fabric_fifos_tx_resources_get(
    int unit);


uint32 dnxf_data_fabric_fifos_rx_full_offset_get(
    int unit);


uint32 dnxf_data_fabric_fifos_mid_full_offset_get(
    int unit);


uint32 dnxf_data_fabric_fifos_tx_full_offset_get(
    int unit);



typedef enum
{
    dnxf_data_fabric_fifos_table_rx_depth_per_pipe,
    dnxf_data_fabric_fifos_table_mid_depth_per_pipe,
    dnxf_data_fabric_fifos_table_tx_depth_per_pipe,

    
    _dnxf_data_fabric_fifos_table_nof
} dnxf_data_fabric_fifos_table_e;



const dnxf_data_fabric_fifos_rx_depth_per_pipe_t * dnxf_data_fabric_fifos_rx_depth_per_pipe_get(
    int unit,
    int pipe_id);


const dnxf_data_fabric_fifos_mid_depth_per_pipe_t * dnxf_data_fabric_fifos_mid_depth_per_pipe_get(
    int unit,
    int pipe_id);


const dnxf_data_fabric_fifos_tx_depth_per_pipe_t * dnxf_data_fabric_fifos_tx_depth_per_pipe_get(
    int unit,
    int pipe_id);



shr_error_e dnxf_data_fabric_fifos_rx_depth_per_pipe_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_fabric_fifos_mid_depth_per_pipe_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_fabric_fifos_tx_depth_per_pipe_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_fabric_fifos_rx_depth_per_pipe_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_fabric_fifos_mid_depth_per_pipe_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_fabric_fifos_tx_depth_per_pipe_info_get(
    int unit);






int dnxf_data_fabric_congestion_feature_get(
    int unit,
    dnxf_data_fabric_congestion_feature_e feature);



typedef enum
{
    dnxf_data_fabric_congestion_define_nof_profiles,
    dnxf_data_fabric_congestion_define_nof_threshold_priorities,
    dnxf_data_fabric_congestion_define_nof_threshold_levels,
    dnxf_data_fabric_congestion_define_nof_threshold_index_dimensions,
    dnxf_data_fabric_congestion_define_nof_thresholds,
    dnxf_data_fabric_congestion_define_nof_dfl_banks,
    dnxf_data_fabric_congestion_define_nof_dfl_sub_banks,
    dnxf_data_fabric_congestion_define_nof_dfl_bank_entries,
    dnxf_data_fabric_congestion_define_nof_rci_bits,

    
    _dnxf_data_fabric_congestion_define_nof
} dnxf_data_fabric_congestion_define_e;



uint32 dnxf_data_fabric_congestion_nof_profiles_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_threshold_priorities_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_threshold_levels_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_threshold_index_dimensions_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_thresholds_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_dfl_banks_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_dfl_sub_banks_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_dfl_bank_entries_get(
    int unit);


uint32 dnxf_data_fabric_congestion_nof_rci_bits_get(
    int unit);



typedef enum
{
    dnxf_data_fabric_congestion_table_thresholds_info,

    
    _dnxf_data_fabric_congestion_table_nof
} dnxf_data_fabric_congestion_table_e;



const dnxf_data_fabric_congestion_thresholds_info_t * dnxf_data_fabric_congestion_thresholds_info_get(
    int unit,
    int threshold_id);



shr_error_e dnxf_data_fabric_congestion_thresholds_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_fabric_congestion_thresholds_info_info_get(
    int unit);






int dnxf_data_fabric_cell_feature_get(
    int unit,
    dnxf_data_fabric_cell_feature_e feature);



typedef enum
{
    dnxf_data_fabric_cell_define_fifo_dma_fabric_cell_nof_entries_per_cell,
    dnxf_data_fabric_cell_define_rx_cpu_cell_max_payload_size,
    dnxf_data_fabric_cell_define_rx_max_nof_cpu_buffers,
    dnxf_data_fabric_cell_define_source_routed_cells_header_offset,
    dnxf_data_fabric_cell_define_fifo_dma_nof_channels,
    dnxf_data_fabric_cell_define_fifo_dma_entry_size,
    dnxf_data_fabric_cell_define_fifo_dma_max_nof_entries,
    dnxf_data_fabric_cell_define_fifo_dma_min_nof_entries,
    dnxf_data_fabric_cell_define_fifo_dma_enable,
    dnxf_data_fabric_cell_define_fifo_dma_buffer_size,
    dnxf_data_fabric_cell_define_fifo_dma_threshold,
    dnxf_data_fabric_cell_define_fifo_dma_timeout,
    dnxf_data_fabric_cell_define_rx_thread_pri,

    
    _dnxf_data_fabric_cell_define_nof
} dnxf_data_fabric_cell_define_e;



uint32 dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_get(
    int unit);


uint32 dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_get(
    int unit);


uint32 dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_get(
    int unit);


uint32 dnxf_data_fabric_cell_source_routed_cells_header_offset_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_nof_channels_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_entry_size_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_max_nof_entries_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_min_nof_entries_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_enable_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_buffer_size_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_threshold_get(
    int unit);


uint32 dnxf_data_fabric_cell_fifo_dma_timeout_get(
    int unit);


uint32 dnxf_data_fabric_cell_rx_thread_pri_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_cell_table_nof
} dnxf_data_fabric_cell_table_e;









int dnxf_data_fabric_topology_feature_get(
    int unit,
    dnxf_data_fabric_topology_feature_e feature);



typedef enum
{
    dnxf_data_fabric_topology_define_max_link_score,
    dnxf_data_fabric_topology_define_nof_local_modid,
    dnxf_data_fabric_topology_define_nof_local_modid_fe13,
    dnxf_data_fabric_topology_define_load_balancing_mode,

    
    _dnxf_data_fabric_topology_define_nof
} dnxf_data_fabric_topology_define_e;



uint32 dnxf_data_fabric_topology_max_link_score_get(
    int unit);


uint32 dnxf_data_fabric_topology_nof_local_modid_get(
    int unit);


uint32 dnxf_data_fabric_topology_nof_local_modid_fe13_get(
    int unit);


uint32 dnxf_data_fabric_topology_load_balancing_mode_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_topology_table_nof
} dnxf_data_fabric_topology_table_e;









int dnxf_data_fabric_gpd_feature_get(
    int unit,
    dnxf_data_fabric_gpd_feature_e feature);



typedef enum
{
    dnxf_data_fabric_gpd_define_in_time,
    dnxf_data_fabric_gpd_define_out_time,

    
    _dnxf_data_fabric_gpd_define_nof
} dnxf_data_fabric_gpd_define_e;



uint32 dnxf_data_fabric_gpd_in_time_get(
    int unit);


uint32 dnxf_data_fabric_gpd_out_time_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_gpd_table_nof
} dnxf_data_fabric_gpd_table_e;









int dnxf_data_fabric_reachability_feature_get(
    int unit,
    dnxf_data_fabric_reachability_feature_e feature);



typedef enum
{
    dnxf_data_fabric_reachability_define_table_row_size_in_uint32,
    dnxf_data_fabric_reachability_define_table_group_size,
    dnxf_data_fabric_reachability_define_rmgr_units,
    dnxf_data_fabric_reachability_define_rmgr_nof_links,
    dnxf_data_fabric_reachability_define_gpd_rmgr_time_factor,
    dnxf_data_fabric_reachability_define_gen_rate_link_delta,
    dnxf_data_fabric_reachability_define_gen_rate_full_cycle,
    dnxf_data_fabric_reachability_define_gpd_gen_rate_full_cycle,
    dnxf_data_fabric_reachability_define_watchdog_rate,

    
    _dnxf_data_fabric_reachability_define_nof
} dnxf_data_fabric_reachability_define_e;



uint32 dnxf_data_fabric_reachability_table_row_size_in_uint32_get(
    int unit);


uint32 dnxf_data_fabric_reachability_table_group_size_get(
    int unit);


uint32 dnxf_data_fabric_reachability_rmgr_units_get(
    int unit);


uint32 dnxf_data_fabric_reachability_rmgr_nof_links_get(
    int unit);


uint32 dnxf_data_fabric_reachability_gpd_rmgr_time_factor_get(
    int unit);


uint32 dnxf_data_fabric_reachability_gen_rate_link_delta_get(
    int unit);


uint32 dnxf_data_fabric_reachability_gen_rate_full_cycle_get(
    int unit);


uint32 dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_get(
    int unit);


uint32 dnxf_data_fabric_reachability_watchdog_rate_get(
    int unit);



typedef enum
{

    
    _dnxf_data_fabric_reachability_table_nof
} dnxf_data_fabric_reachability_table_e;









int dnxf_data_fabric_hw_snake_feature_get(
    int unit,
    dnxf_data_fabric_hw_snake_feature_e feature);



typedef enum
{

    
    _dnxf_data_fabric_hw_snake_define_nof
} dnxf_data_fabric_hw_snake_define_e;




typedef enum
{

    
    _dnxf_data_fabric_hw_snake_table_nof
} dnxf_data_fabric_hw_snake_table_e;






shr_error_e dnxf_data_fabric_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

