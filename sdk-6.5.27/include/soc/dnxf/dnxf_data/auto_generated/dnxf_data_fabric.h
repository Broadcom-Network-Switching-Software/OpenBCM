
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_FABRIC_H_

#define _DNXF_DATA_FABRIC_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/types.h>
#include <soc/dnxc/fabric.h>
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_fabric_init(
    int unit);







typedef enum
{
    dnxf_data_fabric_general_is_multi_stage_supported,
    dnxf_data_fabric_general_is_symb_errors_per_frame_supported,
    dnxf_data_fabric_general_is_nrz_supported,

    
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
    int lr_uc_pipe;
    char *name;
    soc_dnxc_fabric_pipe_map_type_t type;
    int min_hp_mc;
} dnxf_data_fabric_pipes_map_t;


typedef struct
{
    uint32 nof_pipes;
    uint32 uc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    uint32 mc[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES];
    int lr_uc_pipe;
    char *name;
    soc_dnxc_fabric_pipe_map_type_t type;
    int min_hp_mc;
} dnxf_data_fabric_pipes_valid_map_config_t;



typedef enum
{
    dnxf_data_fabric_pipes_local_route_pipe_supported,

    
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
    *dnxf_data_fabric_pipes_local_route_pipe_index_get_f) (
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
    
    dnxf_data_fabric_pipes_local_route_pipe_index_get_f local_route_pipe_index_get;
    
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
    dnxf_data_fabric_multicast_egress_api_supported,
    dnxf_data_fabric_multicast_mc_prio_per_mc_range_supported,
    dnxf_data_fabric_multicast_static_mode_supported,

    
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
    dnxf_data_fabric_fifos_reduced_buffers,

    
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
    *dnxf_data_fabric_fifos_reduced_rx_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_dfl_resources_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_reduced_dfl_resources_get_f) (
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


typedef uint32(
    *dnxf_data_fabric_fifos_dfl_bank_wr_threshold_offset_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifos_dfl_bank_admit_threshold_additional_offset_get_f) (
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
    
    dnxf_data_fabric_fifos_reduced_rx_resources_get_f reduced_rx_resources_get;
    
    dnxf_data_fabric_fifos_dfl_resources_get_f dfl_resources_get;
    
    dnxf_data_fabric_fifos_reduced_dfl_resources_get_f reduced_dfl_resources_get;
    
    dnxf_data_fabric_fifos_rx_full_offset_get_f rx_full_offset_get;
    
    dnxf_data_fabric_fifos_mid_full_offset_get_f mid_full_offset_get;
    
    dnxf_data_fabric_fifos_tx_full_offset_get_f tx_full_offset_get;
    
    dnxf_data_fabric_fifos_dfl_bank_wr_threshold_offset_get_f dfl_bank_wr_threshold_offset_get;
    
    dnxf_data_fabric_fifos_dfl_bank_admit_threshold_additional_offset_get_f dfl_bank_admit_threshold_additional_offset_get;
    
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
    uint32 is_valid;
} dnxf_data_fabric_congestion_thresholds_info_t;



typedef enum
{
    dnxf_data_fabric_congestion_credit_based_llfc_supported,

    
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


typedef uint32(
    *dnxf_data_fabric_congestion_max_wfq_weight_value_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_congestion_llfc_type_default_get_f) (
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
    
    dnxf_data_fabric_congestion_nof_dfl_banks_get_f nof_dfl_banks_get;
    
    dnxf_data_fabric_congestion_nof_dfl_sub_banks_get_f nof_dfl_sub_banks_get;
    
    dnxf_data_fabric_congestion_nof_dfl_bank_entries_get_f nof_dfl_bank_entries_get;
    
    dnxf_data_fabric_congestion_nof_rci_bits_get_f nof_rci_bits_get;
    
    dnxf_data_fabric_congestion_max_wfq_weight_value_get_f max_wfq_weight_value_get;
    
    dnxf_data_fabric_congestion_llfc_type_default_get_f llfc_type_default_get;
    
    dnxf_data_fabric_congestion_thresholds_info_get_f thresholds_info_get;
    
    dnxc_data_table_info_get_f thresholds_info_info_get;
} dnxf_data_if_fabric_congestion_t;







typedef enum
{

    
    _dnxf_data_fabric_fifo_dma_feature_nof
} dnxf_data_fabric_fifo_dma_feature_e;



typedef int(
    *dnxf_data_fabric_fifo_dma_feature_get_f) (
    int unit,
    dnxf_data_fabric_fifo_dma_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_fabric_cell_nof_entries_per_cell_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_nof_channels_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_max_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_min_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_threshold_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_fifo_dma_timeout_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_fifo_dma_feature_get_f feature_get;
    
    dnxf_data_fabric_fifo_dma_fabric_cell_nof_entries_per_cell_get_f fabric_cell_nof_entries_per_cell_get;
    
    dnxf_data_fabric_fifo_dma_nof_channels_get_f nof_channels_get;
    
    dnxf_data_fabric_fifo_dma_entry_size_get_f entry_size_get;
    
    dnxf_data_fabric_fifo_dma_max_nof_entries_get_f max_nof_entries_get;
    
    dnxf_data_fabric_fifo_dma_min_nof_entries_get_f min_nof_entries_get;
    
    dnxf_data_fabric_fifo_dma_enable_get_f enable_get;
    
    dnxf_data_fabric_fifo_dma_buffer_size_get_f buffer_size_get;
    
    dnxf_data_fabric_fifo_dma_threshold_get_f threshold_get;
    
    dnxf_data_fabric_fifo_dma_timeout_get_f timeout_get;
} dnxf_data_if_fabric_fifo_dma_t;






typedef struct
{
    int dma_channel;
} dnxf_data_fabric_rxtx_tx_dma_channel_info_t;


typedef struct
{
    int dma_channel;
    bcm_pbmp_t cos;
} dnxf_data_fabric_rxtx_rx_dma_channel_info_t;



typedef enum
{
    dnxf_data_fabric_rxtx_pkt_dma_supported,

    
    _dnxf_data_fabric_rxtx_feature_nof
} dnxf_data_fabric_rxtx_feature_e;



typedef int(
    *dnxf_data_fabric_rxtx_feature_get_f) (
    int unit,
    dnxf_data_fabric_rxtx_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_rxtx_nof_tx_channels_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_rxtx_nof_ep_ip_instances_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_rxtx_pktsrc_id_to_channel_bit_field_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_rxtx_pktsrc_id_to_dest_field_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_rxtx_max_nof_sub_channels_used_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_rxtx_rx_pool_nof_pkts_get_f) (
    int unit);


typedef const dnxf_data_fabric_rxtx_tx_dma_channel_info_t *(
    *dnxf_data_fabric_rxtx_tx_dma_channel_info_get_f) (
    int unit,
    int channel_type,
    int index);


typedef const dnxf_data_fabric_rxtx_rx_dma_channel_info_t *(
    *dnxf_data_fabric_rxtx_rx_dma_channel_info_get_f) (
    int unit,
    int channel_type);



typedef struct
{
    
    dnxf_data_fabric_rxtx_feature_get_f feature_get;
    
    dnxf_data_fabric_rxtx_nof_tx_channels_get_f nof_tx_channels_get;
    
    dnxf_data_fabric_rxtx_nof_ep_ip_instances_get_f nof_ep_ip_instances_get;
    
    dnxf_data_fabric_rxtx_pktsrc_id_to_channel_bit_field_size_get_f pktsrc_id_to_channel_bit_field_size_get;
    
    dnxf_data_fabric_rxtx_pktsrc_id_to_dest_field_size_get_f pktsrc_id_to_dest_field_size_get;
    
    dnxf_data_fabric_rxtx_max_nof_sub_channels_used_get_f max_nof_sub_channels_used_get;
    
    dnxf_data_fabric_rxtx_rx_pool_nof_pkts_get_f rx_pool_nof_pkts_get;
    
    dnxf_data_fabric_rxtx_tx_dma_channel_info_get_f tx_dma_channel_info_get;
    
    dnxc_data_table_info_get_f tx_dma_channel_info_info_get;
    
    dnxf_data_fabric_rxtx_rx_dma_channel_info_get_f rx_dma_channel_info_get;
    
    dnxc_data_table_info_get_f rx_dma_channel_info_info_get;
} dnxf_data_if_fabric_rxtx_t;






typedef struct
{
    uint32 start_bit;
    uint32 bit_length;
    uint32 mask;
} dnxf_data_fabric_cell_cell_match_filter_reg_mask_t;



typedef enum
{
    dnxf_data_fabric_cell_format_always_with_pcp,
    dnxf_data_fabric_cell_fe2_control_cells_filter_required,

    
    _dnxf_data_fabric_cell_feature_nof
} dnxf_data_fabric_cell_feature_e;



typedef int(
    *dnxf_data_fabric_cell_feature_get_f) (
    int unit,
    dnxf_data_fabric_cell_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_cell_max_vsc_format_size_get_f) (
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
    *dnxf_data_fabric_cell_rx_thread_pri_get_f) (
    int unit);


typedef const dnxf_data_fabric_cell_cell_match_filter_reg_mask_t *(
    *dnxf_data_fabric_cell_cell_match_filter_reg_mask_get_f) (
    int unit,
    int filter_type);



typedef struct
{
    
    dnxf_data_fabric_cell_feature_get_f feature_get;
    
    dnxf_data_fabric_cell_max_vsc_format_size_get_f max_vsc_format_size_get;
    
    dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_get_f rx_cpu_cell_max_payload_size_get;
    
    dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_get_f rx_max_nof_cpu_buffers_get;
    
    dnxf_data_fabric_cell_source_routed_cells_header_offset_get_f source_routed_cells_header_offset_get;
    
    dnxf_data_fabric_cell_rx_thread_pri_get_f rx_thread_pri_get;
    
    dnxf_data_fabric_cell_cell_match_filter_reg_mask_get_f cell_match_filter_reg_mask_get;
    
    dnxc_data_table_info_get_f cell_match_filter_reg_mask_info_get;
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
    *dnxf_data_fabric_topology_nof_system_modid_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_nof_local_modid_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_nof_local_modid_fe13_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_all_reachable_fap_group_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_all_reachable_max_nof_ignore_faps_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_topology_load_balancing_mode_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_topology_feature_get_f feature_get;
    
    dnxf_data_fabric_topology_max_link_score_get_f max_link_score_get;
    
    dnxf_data_fabric_topology_nof_system_modid_get_f nof_system_modid_get;
    
    dnxf_data_fabric_topology_nof_local_modid_get_f nof_local_modid_get;
    
    dnxf_data_fabric_topology_nof_local_modid_fe13_get_f nof_local_modid_fe13_get;
    
    dnxf_data_fabric_topology_all_reachable_fap_group_size_get_f all_reachable_fap_group_size_get;
    
    dnxf_data_fabric_topology_all_reachable_max_nof_ignore_faps_get_f all_reachable_max_nof_ignore_faps_get;
    
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
    uint32 value;
} dnxf_data_fabric_system_upgrade_target_t;



typedef enum
{
    dnxf_data_fabric_system_upgrade_is_system_upgrade_supported,

    
    _dnxf_data_fabric_system_upgrade_feature_nof
} dnxf_data_fabric_system_upgrade_feature_e;



typedef int(
    *dnxf_data_fabric_system_upgrade_feature_get_f) (
    int unit,
    dnxf_data_fabric_system_upgrade_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_max_master_id_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_0_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_1_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_2_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_3_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_4_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_5_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_fabric_system_upgrade_param_7_get_f) (
    int unit);


typedef const dnxf_data_fabric_system_upgrade_target_t *(
    *dnxf_data_fabric_system_upgrade_target_get_f) (
    int unit,
    int target);



typedef struct
{
    
    dnxf_data_fabric_system_upgrade_feature_get_f feature_get;
    
    dnxf_data_fabric_system_upgrade_max_master_id_get_f max_master_id_get;
    
    dnxf_data_fabric_system_upgrade_param_0_get_f param_0_get;
    
    dnxf_data_fabric_system_upgrade_param_1_get_f param_1_get;
    
    dnxf_data_fabric_system_upgrade_param_2_get_f param_2_get;
    
    dnxf_data_fabric_system_upgrade_param_3_get_f param_3_get;
    
    dnxf_data_fabric_system_upgrade_param_4_get_f param_4_get;
    
    dnxf_data_fabric_system_upgrade_param_5_get_f param_5_get;
    
    dnxf_data_fabric_system_upgrade_param_7_get_f param_7_get;
    
    dnxf_data_fabric_system_upgrade_target_get_f target_get;
    
    dnxc_data_table_info_get_f target_info_get;
} dnxf_data_if_fabric_system_upgrade_t;







typedef enum
{

    
    _dnxf_data_fabric_regression_feature_nof
} dnxf_data_fabric_regression_feature_e;



typedef int(
    *dnxf_data_fabric_regression_feature_get_f) (
    int unit,
    dnxf_data_fabric_regression_feature_e feature);


typedef uint32(
    *dnxf_data_fabric_regression_minimum_system_ref_clk_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_fabric_regression_feature_get_f feature_get;
    
    dnxf_data_fabric_regression_minimum_system_ref_clk_get_f minimum_system_ref_clk_get;
} dnxf_data_if_fabric_regression_t;





typedef struct
{
    
    dnxf_data_if_fabric_general_t general;
    
    dnxf_data_if_fabric_pipes_t pipes;
    
    dnxf_data_if_fabric_multicast_t multicast;
    
    dnxf_data_if_fabric_fifos_t fifos;
    
    dnxf_data_if_fabric_congestion_t congestion;
    
    dnxf_data_if_fabric_fifo_dma_t fifo_dma;
    
    dnxf_data_if_fabric_rxtx_t rxtx;
    
    dnxf_data_if_fabric_cell_t cell;
    
    dnxf_data_if_fabric_topology_t topology;
    
    dnxf_data_if_fabric_gpd_t gpd;
    
    dnxf_data_if_fabric_reachability_t reachability;
    
    dnxf_data_if_fabric_hw_snake_t hw_snake;
    
    dnxf_data_if_fabric_system_upgrade_t system_upgrade;
    
    dnxf_data_if_fabric_regression_t regression;
} dnxf_data_if_fabric_t;




extern dnxf_data_if_fabric_t dnxf_data_fabric;


#endif 

