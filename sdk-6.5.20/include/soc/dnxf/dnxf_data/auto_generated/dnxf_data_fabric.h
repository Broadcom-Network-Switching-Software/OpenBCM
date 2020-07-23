

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_FABRIC_H_

#define _DNXF_DATA_FABRIC_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/types.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_fabric_init(
    int unit);







typedef enum
{
    
    dnxf_data_fabric_general_is_multi_stage_supported,

    
    _dnxf_data_fabric_general_feature_nof
} dnxf_data_fabric_general_feature_e;



typedef int(
    *dnxf_data_fabric_general_feature_get_f) (
    int unit,
    dnxf_data_fabric_general_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_general_max_nof_pipes_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_general_device_mode_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_general_local_routing_enable_uc_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_general_local_routing_enable_mc_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_general_feature_get_f feature_get;
    
    dnxf_data_fabric_general_max_nof_pipes_get_f max_nof_pipes_get;
    
    dnxf_data_fabric_general_device_mode_get_f device_mode_get;
    
    dnxf_data_fabric_general_local_routing_enable_uc_get_f local_routing_enable_uc_get;
    
    dnxf_data_fabric_general_local_routing_enable_mc_get_f local_routing_enable_mc_get;
} dnxf_data_if_fabric_general_t;






typedef struct
{
    
    uint32 uc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    
    uint32 mc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    
    char *name;
    
    soc_dnxc_fabric_pipe_map_type_t type;
    
    int min_hp_mc;
} dnxf_data_fabric_pipes_map_t;


typedef struct
{
    
    uint32 nof_pipes;
    
    uint32 uc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    
    uint32 mc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    
    char *name;
    
    soc_dnxc_fabric_pipe_map_type_t type;
    
    int min_hp_mc;
} dnxf_data_fabric_pipes_valid_map_config_t;



typedef enum
{

    
    _dnxf_data_fabric_pipes_feature_nof
} dnxf_data_fabric_pipes_feature_e;



typedef int(
    *dnxf_data_fabric_pipes_feature_get_f) (
    int unit,
    dnxf_data_fabric_pipes_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_pipes_max_nof_pipes_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_pipes_nof_valid_mapping_options_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_pipes_nof_pipes_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_pipes_system_tdm_priority_get_f) (
    int unit);


typedef const dnxf_data_fabric_pipes_map_t *(
    *dnxf_data_fabric_pipes_map_get_f) (
    int unit);


typedef const dnxf_data_fabric_pipes_valid_map_config_t *(
    *dnxf_data_fabric_pipes_valid_map_config_get_f) (
    int unit,
    int mapping_option_idx);



typedef struct
{
    
    dnxf_data_fabric_pipes_feature_get_f feature_get;
    
    dnxf_data_fabric_pipes_max_nof_pipes_get_f max_nof_pipes_get;
    
    dnxf_data_fabric_pipes_nof_valid_mapping_options_get_f nof_valid_mapping_options_get;
    
    dnxf_data_fabric_pipes_nof_pipes_get_f nof_pipes_get;
    
    dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_get_f system_contains_multiple_pipe_device_get;
    
    dnxf_data_fabric_pipes_system_tdm_priority_get_f system_tdm_priority_get;
    
    dnxf_data_fabric_pipes_map_get_f map_get;
    
    dnxc_data_table_info_get_f map_info_get;
    
    dnxf_data_fabric_pipes_valid_map_config_get_f valid_map_config_get;
    
    dnxc_data_table_info_get_f valid_map_config_info_get;
} dnxf_data_if_fabric_pipes_t;







typedef enum
{

    
    _dnxf_data_fabric_multicast_feature_nof
} dnxf_data_fabric_multicast_feature_e;



typedef int(
    *dnxf_data_fabric_multicast_feature_get_f) (
    int unit,
    dnxf_data_fabric_multicast_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_multicast_table_row_size_in_uint32_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_multicast_id_range_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_multicast_mode_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_multicast_priority_map_enable_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_multicast_feature_get_f feature_get;
    
    dnxf_data_fabric_multicast_table_row_size_in_uint32_get_f table_row_size_in_uint32_get;
    
    dnxf_data_fabric_multicast_id_range_get_f id_range_get;
    
    dnxf_data_fabric_multicast_mode_get_f mode_get;
    
    dnxf_data_fabric_multicast_priority_map_enable_get_f priority_map_enable_get;
} dnxf_data_if_fabric_multicast_t;






typedef struct
{
    
    uint32 fe1;
    
    uint32 fe3;
} dnxf_data_fabric_fifos_rx_depth_per_pipe_t;


typedef struct
{
    
    uint32 fe1_nlr;
    
    uint32 fe3_nlr;
    
    uint32 fe1_lr;
    
    uint32 fe3_lr;
} dnxf_data_fabric_fifos_mid_depth_per_pipe_t;


typedef struct
{
    
    uint32 fe1_nlr;
    
    uint32 fe3_nlr;
    
    uint32 fe1_lr;
    
    uint32 fe3_lr;
} dnxf_data_fabric_fifos_tx_depth_per_pipe_t;



typedef enum
{

    
    _dnxf_data_fabric_fifos_feature_nof
} dnxf_data_fabric_fifos_feature_e;



typedef int(
    *dnxf_data_fabric_fifos_feature_get_f) (
    int unit,
    dnxf_data_fabric_fifos_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_fifos_granularity_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_min_depth_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_max_unused_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_rx_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_mid_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_tx_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_rx_full_offset_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_mid_full_offset_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_tx_full_offset_get_f) (
    int unit);


typedef const dnxf_data_fabric_fifos_rx_depth_per_pipe_t *(
    *dnxf_data_fabric_fifos_rx_depth_per_pipe_get_f) (
    int unit,
    int pipe_id);


typedef const dnxf_data_fabric_fifos_mid_depth_per_pipe_t *(
    *dnxf_data_fabric_fifos_mid_depth_per_pipe_get_f) (
    int unit,
    int pipe_id);


typedef const dnxf_data_fabric_fifos_tx_depth_per_pipe_t *(
    *dnxf_data_fabric_fifos_tx_depth_per_pipe_get_f) (
    int unit,
    int pipe_id);



typedef struct
{
    
    dnxf_data_fabric_fifos_feature_get_f feature_get;
    
    dnxf_data_fabric_fifos_granularity_get_f granularity_get;
    
    dnxf_data_fabric_fifos_min_depth_get_f min_depth_get;
    
    dnxf_data_fabric_fifos_max_unused_resources_get_f max_unused_resources_get;
    
    dnxf_data_fabric_fifos_rx_resources_get_f rx_resources_get;
    
    dnxf_data_fabric_fifos_mid_resources_get_f mid_resources_get;
    
    dnxf_data_fabric_fifos_tx_resources_get_f tx_resources_get;
    
    dnxf_data_fabric_fifos_rx_full_offset_get_f rx_full_offset_get;
    
    dnxf_data_fabric_fifos_mid_full_offset_get_f mid_full_offset_get;
    
    dnxf_data_fabric_fifos_tx_full_offset_get_f tx_full_offset_get;
    
    dnxf_data_fabric_fifos_rx_depth_per_pipe_get_f rx_depth_per_pipe_get;
    
    dnxc_data_table_info_get_f rx_depth_per_pipe_info_get;
    
    dnxf_data_fabric_fifos_mid_depth_per_pipe_get_f mid_depth_per_pipe_get;
    
    dnxc_data_table_info_get_f mid_depth_per_pipe_info_get;
    
    dnxf_data_fabric_fifos_tx_depth_per_pipe_get_f tx_depth_per_pipe_get;
    
    dnxc_data_table_info_get_f tx_depth_per_pipe_info_get;
} dnxf_data_if_fabric_fifos_t;






typedef struct
{
    
    char *threshold_str;
    
    soc_dnxf_threshold_group_options_t threshold_group;
    
    soc_dnxf_threshold_stage_options_t threshold_stage;
    
    _shr_dnxf_cgm_index_type_t index_type;
    
    uint32 supported_flags;
    
    uint32 is_tx;
    
    uint32 th_validity_bmp;
    
    uint32 is_to_be_displayed;
} dnxf_data_fabric_congestion_thresholds_info_t;



typedef enum
{

    
    _dnxf_data_fabric_congestion_feature_nof
} dnxf_data_fabric_congestion_feature_e;



typedef int(
    *dnxf_data_fabric_congestion_feature_get_f) (
    int unit,
    dnxf_data_fabric_congestion_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_threshold_priorities_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_threshold_levels_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_threshold_index_dimensions_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_thresholds_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_dfl_banks_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_dfl_sub_banks_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_dfl_bank_entries_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_nof_rci_bits_get_f) (
    int unit);


typedef const dnxf_data_fabric_congestion_thresholds_info_t *(
    *dnxf_data_fabric_congestion_thresholds_info_get_f) (
    int unit,
    int threshold_id);



typedef struct
{
    
    dnxf_data_fabric_congestion_feature_get_f feature_get;
    
    dnxf_data_fabric_congestion_nof_profiles_get_f nof_profiles_get;
    
    dnxf_data_fabric_congestion_nof_threshold_priorities_get_f nof_threshold_priorities_get;
    
    dnxf_data_fabric_congestion_nof_threshold_levels_get_f nof_threshold_levels_get;
    
    dnxf_data_fabric_congestion_nof_threshold_index_dimensions_get_f nof_threshold_index_dimensions_get;
    
    dnxf_data_fabric_congestion_nof_thresholds_get_f nof_thresholds_get;
    
    dnxf_data_fabric_congestion_nof_dfl_banks_get_f nof_dfl_banks_get;
    
    dnxf_data_fabric_congestion_nof_dfl_sub_banks_get_f nof_dfl_sub_banks_get;
    
    dnxf_data_fabric_congestion_nof_dfl_bank_entries_get_f nof_dfl_bank_entries_get;
    
    dnxf_data_fabric_congestion_nof_rci_bits_get_f nof_rci_bits_get;
    
    dnxf_data_fabric_congestion_thresholds_info_get_f thresholds_info_get;
    
    dnxc_data_table_info_get_f thresholds_info_info_get;
} dnxf_data_if_fabric_congestion_t;







typedef enum
{

    
    _dnxf_data_fabric_cell_feature_nof
} dnxf_data_fabric_cell_feature_e;



typedef int(
    *dnxf_data_fabric_cell_feature_get_f) (
    int unit,
    dnxf_data_fabric_cell_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_source_routed_cells_header_offset_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_nof_channels_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_max_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_min_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_threshold_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_fifo_dma_timeout_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_cell_rx_thread_pri_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_cell_feature_get_f feature_get;
    
    dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_get_f fifo_dma_fabric_cell_nof_entries_per_cell_get;
    
    dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_get_f rx_cpu_cell_max_payload_size_get;
    
    dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_get_f rx_max_nof_cpu_buffers_get;
    
    dnxf_data_fabric_cell_source_routed_cells_header_offset_get_f source_routed_cells_header_offset_get;
    
    dnxf_data_fabric_cell_fifo_dma_nof_channels_get_f fifo_dma_nof_channels_get;
    
    dnxf_data_fabric_cell_fifo_dma_entry_size_get_f fifo_dma_entry_size_get;
    
    dnxf_data_fabric_cell_fifo_dma_max_nof_entries_get_f fifo_dma_max_nof_entries_get;
    
    dnxf_data_fabric_cell_fifo_dma_min_nof_entries_get_f fifo_dma_min_nof_entries_get;
    
    dnxf_data_fabric_cell_fifo_dma_enable_get_f fifo_dma_enable_get;
    
    dnxf_data_fabric_cell_fifo_dma_buffer_size_get_f fifo_dma_buffer_size_get;
    
    dnxf_data_fabric_cell_fifo_dma_threshold_get_f fifo_dma_threshold_get;
    
    dnxf_data_fabric_cell_fifo_dma_timeout_get_f fifo_dma_timeout_get;
    
    dnxf_data_fabric_cell_rx_thread_pri_get_f rx_thread_pri_get;
} dnxf_data_if_fabric_cell_t;







typedef enum
{

    
    _dnxf_data_fabric_topology_feature_nof
} dnxf_data_fabric_topology_feature_e;



typedef int(
    *dnxf_data_fabric_topology_feature_get_f) (
    int unit,
    dnxf_data_fabric_topology_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_topology_max_link_score_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_nof_local_modid_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_nof_local_modid_fe13_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_load_balancing_mode_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_topology_feature_get_f feature_get;
    
    dnxf_data_fabric_topology_max_link_score_get_f max_link_score_get;
    
    dnxf_data_fabric_topology_nof_local_modid_get_f nof_local_modid_get;
    
    dnxf_data_fabric_topology_nof_local_modid_fe13_get_f nof_local_modid_fe13_get;
    
    dnxf_data_fabric_topology_load_balancing_mode_get_f load_balancing_mode_get;
} dnxf_data_if_fabric_topology_t;







typedef enum
{

    
    _dnxf_data_fabric_gpd_feature_nof
} dnxf_data_fabric_gpd_feature_e;



typedef int(
    *dnxf_data_fabric_gpd_feature_get_f) (
    int unit,
    dnxf_data_fabric_gpd_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_gpd_in_time_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_gpd_out_time_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_gpd_feature_get_f feature_get;
    
    dnxf_data_fabric_gpd_in_time_get_f in_time_get;
    
    dnxf_data_fabric_gpd_out_time_get_f out_time_get;
} dnxf_data_if_fabric_gpd_t;







typedef enum
{

    
    _dnxf_data_fabric_reachability_feature_nof
} dnxf_data_fabric_reachability_feature_e;



typedef int(
    *dnxf_data_fabric_reachability_feature_get_f) (
    int unit,
    dnxf_data_fabric_reachability_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_reachability_table_row_size_in_uint32_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_table_group_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_rmgr_units_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_rmgr_nof_links_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_gpd_rmgr_time_factor_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_gen_rate_link_delta_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_gen_rate_full_cycle_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_reachability_watchdog_rate_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_reachability_feature_get_f feature_get;
    
    dnxf_data_fabric_reachability_table_row_size_in_uint32_get_f table_row_size_in_uint32_get;
    
    dnxf_data_fabric_reachability_table_group_size_get_f table_group_size_get;
    
    dnxf_data_fabric_reachability_rmgr_units_get_f rmgr_units_get;
    
    dnxf_data_fabric_reachability_rmgr_nof_links_get_f rmgr_nof_links_get;
    
    dnxf_data_fabric_reachability_gpd_rmgr_time_factor_get_f gpd_rmgr_time_factor_get;
    
    dnxf_data_fabric_reachability_gen_rate_link_delta_get_f gen_rate_link_delta_get;
    
    dnxf_data_fabric_reachability_gen_rate_full_cycle_get_f gen_rate_full_cycle_get;
    
    dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_get_f gpd_gen_rate_full_cycle_get;
    
    dnxf_data_fabric_reachability_watchdog_rate_get_f watchdog_rate_get;
} dnxf_data_if_fabric_reachability_t;







typedef enum
{
    
    dnxf_data_fabric_hw_snake_is_sw_config_required,

    
    _dnxf_data_fabric_hw_snake_feature_nof
} dnxf_data_fabric_hw_snake_feature_e;



typedef int(
    *dnxf_data_fabric_hw_snake_feature_get_f) (
    int unit,
    dnxf_data_fabric_hw_snake_feature_e feature);



typedef struct
{
    
    dnxf_data_fabric_hw_snake_feature_get_f feature_get;
} dnxf_data_if_fabric_hw_snake_t;





typedef struct
{
    
    dnxf_data_if_fabric_general_t general;
    
    dnxf_data_if_fabric_pipes_t pipes;
    
    dnxf_data_if_fabric_multicast_t multicast;
    
    dnxf_data_if_fabric_fifos_t fifos;
    
    dnxf_data_if_fabric_congestion_t congestion;
    
    dnxf_data_if_fabric_cell_t cell;
    
    dnxf_data_if_fabric_topology_t topology;
    
    dnxf_data_if_fabric_gpd_t gpd;
    
    dnxf_data_if_fabric_reachability_t reachability;
    
    dnxf_data_if_fabric_hw_snake_t hw_snake;
} dnxf_data_if_fabric_t;




extern dnxf_data_if_fabric_t dnxf_data_fabric;


#endif 

