
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FABRIC_H_

#define _DNX_DATA_INTERNAL_FABRIC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_fabric_submodule_general,
    dnx_data_fabric_submodule_links,
    dnx_data_fabric_submodule_blocks,
    dnx_data_fabric_submodule_reachability,
    dnx_data_fabric_submodule_mesh,
    dnx_data_fabric_submodule_cell,
    dnx_data_fabric_submodule_pipes,
    dnx_data_fabric_submodule_debug,
    dnx_data_fabric_submodule_mesh_topology,
    dnx_data_fabric_submodule_tdm,
    dnx_data_fabric_submodule_cgm,
    dnx_data_fabric_submodule_dtqs,
    dnx_data_fabric_submodule_ilkn,
    dnx_data_fabric_submodule_dbal,
    dnx_data_fabric_submodule_mesh_mc,
    dnx_data_fabric_submodule_transmit,
    dnx_data_fabric_submodule_system_upgrade,
    dnx_data_fabric_submodule_regression,

    
    _dnx_data_fabric_submodule_nof
} dnx_data_fabric_submodule_e;








int dnx_data_fabric_general_feature_get(
    int unit,
    dnx_data_fabric_general_feature_e feature);



typedef enum
{
    dnx_data_fabric_general_define_nof_lcplls,
    dnx_data_fabric_general_define_fmac_clock_khz,
    dnx_data_fabric_general_define_fmac_tx_padding_min_port_speed,
    dnx_data_fabric_general_define_nof_ber_proj_error_analyzer_counters,
    dnx_data_fabric_general_define_connect_mode,

    
    _dnx_data_fabric_general_define_nof
} dnx_data_fabric_general_define_e;



uint32 dnx_data_fabric_general_nof_lcplls_get(
    int unit);


uint32 dnx_data_fabric_general_fmac_clock_khz_get(
    int unit);


uint32 dnx_data_fabric_general_fmac_tx_padding_min_port_speed_get(
    int unit);


uint32 dnx_data_fabric_general_nof_ber_proj_error_analyzer_counters_get(
    int unit);


uint32 dnx_data_fabric_general_connect_mode_get(
    int unit);



typedef enum
{
    dnx_data_fabric_general_table_fmac_bus_size,
    dnx_data_fabric_general_table_pll_phys,
    dnx_data_fabric_general_table_pm_properties,
    dnx_data_fabric_general_table_force_single_pll,
    dnx_data_fabric_general_table_supported_fmac_tx_padding_sizes,

    
    _dnx_data_fabric_general_table_nof
} dnx_data_fabric_general_table_e;



const dnx_data_fabric_general_fmac_bus_size_t * dnx_data_fabric_general_fmac_bus_size_get(
    int unit,
    int mode);


const dnx_data_fabric_general_pll_phys_t * dnx_data_fabric_general_pll_phys_get(
    int unit,
    int lcpll);


const dnx_data_fabric_general_pm_properties_t * dnx_data_fabric_general_pm_properties_get(
    int unit,
    int pm_index);


const dnx_data_fabric_general_force_single_pll_t * dnx_data_fabric_general_force_single_pll_get(
    int unit,
    int pm);


const dnx_data_fabric_general_supported_fmac_tx_padding_sizes_t * dnx_data_fabric_general_supported_fmac_tx_padding_sizes_get(
    int unit,
    int index);



shr_error_e dnx_data_fabric_general_fmac_bus_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_general_pll_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_general_pm_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_general_force_single_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_general_supported_fmac_tx_padding_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_general_fmac_bus_size_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_general_pll_phys_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_general_pm_properties_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_general_force_single_pll_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_general_supported_fmac_tx_padding_sizes_info_get(
    int unit);






int dnx_data_fabric_links_feature_get(
    int unit,
    dnx_data_fabric_links_feature_e feature);



typedef enum
{
    dnx_data_fabric_links_define_nof_links,
    dnx_data_fabric_links_define_nof_links_per_core,
    dnx_data_fabric_links_define_max_link_id,
    dnx_data_fabric_links_define_usec_between_load_balancing_events,
    dnx_data_fabric_links_define_kr_fec_supported,
    dnx_data_fabric_links_define_wfd_max_hw_weight,
    dnx_data_fabric_links_define_load_balancing_profile_id_min,
    dnx_data_fabric_links_define_load_balancing_profile_id_max,
    dnx_data_fabric_links_define_fabric_pm_type,
    dnx_data_fabric_links_define_core_mapping_mode,

    
    _dnx_data_fabric_links_define_nof
} dnx_data_fabric_links_define_e;



uint32 dnx_data_fabric_links_nof_links_get(
    int unit);


uint32 dnx_data_fabric_links_nof_links_per_core_get(
    int unit);


uint32 dnx_data_fabric_links_max_link_id_get(
    int unit);


uint32 dnx_data_fabric_links_usec_between_load_balancing_events_get(
    int unit);


uint32 dnx_data_fabric_links_kr_fec_supported_get(
    int unit);


uint32 dnx_data_fabric_links_wfd_max_hw_weight_get(
    int unit);


uint32 dnx_data_fabric_links_load_balancing_profile_id_min_get(
    int unit);


uint32 dnx_data_fabric_links_load_balancing_profile_id_max_get(
    int unit);


uint32 dnx_data_fabric_links_fabric_pm_type_get(
    int unit);


uint32 dnx_data_fabric_links_core_mapping_mode_get(
    int unit);



typedef enum
{
    dnx_data_fabric_links_table_general,
    dnx_data_fabric_links_table_polarity,
    dnx_data_fabric_links_table_supported_interfaces,
    dnx_data_fabric_links_table_default_fec,
    dnx_data_fabric_links_table_supported_fec_controls,

    
    _dnx_data_fabric_links_table_nof
} dnx_data_fabric_links_table_e;



const dnx_data_fabric_links_general_t * dnx_data_fabric_links_general_get(
    int unit);


const dnx_data_fabric_links_polarity_t * dnx_data_fabric_links_polarity_get(
    int unit,
    int link);


const dnx_data_fabric_links_supported_interfaces_t * dnx_data_fabric_links_supported_interfaces_get(
    int unit,
    int index);


const dnx_data_fabric_links_default_fec_t * dnx_data_fabric_links_default_fec_get(
    int unit,
    int speed);


const dnx_data_fabric_links_supported_fec_controls_t * dnx_data_fabric_links_supported_fec_controls_get(
    int unit,
    int index);



shr_error_e dnx_data_fabric_links_general_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_links_polarity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_links_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_links_default_fec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_links_supported_fec_controls_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_links_general_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_links_polarity_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_links_supported_interfaces_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_links_default_fec_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_links_supported_fec_controls_info_get(
    int unit);






int dnx_data_fabric_blocks_feature_get(
    int unit,
    dnx_data_fabric_blocks_feature_e feature);



typedef enum
{
    dnx_data_fabric_blocks_define_nof_instances_fmac,
    dnx_data_fabric_blocks_define_nof_links_in_fmac,
    dnx_data_fabric_blocks_define_nof_instances_fsrd,
    dnx_data_fabric_blocks_define_nof_links_in_fsrd,
    dnx_data_fabric_blocks_define_nof_fmacs_in_fsrd,
    dnx_data_fabric_blocks_define_nof_pms,
    dnx_data_fabric_blocks_define_nof_links_in_pm,
    dnx_data_fabric_blocks_define_nof_fmacs_in_pm,
    dnx_data_fabric_blocks_define_nof_instances_fdtl,
    dnx_data_fabric_blocks_define_nof_links_in_fdtl,
    dnx_data_fabric_blocks_define_nof_sbus_chains,

    
    _dnx_data_fabric_blocks_define_nof
} dnx_data_fabric_blocks_define_e;



uint32 dnx_data_fabric_blocks_nof_instances_fmac_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_links_in_fmac_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_instances_fsrd_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_links_in_fsrd_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_fmacs_in_fsrd_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_pms_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_links_in_pm_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_fmacs_in_pm_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_instances_fdtl_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_links_in_fdtl_get(
    int unit);


uint32 dnx_data_fabric_blocks_nof_sbus_chains_get(
    int unit);



typedef enum
{
    dnx_data_fabric_blocks_table_fsrd_sbus_chain,
    dnx_data_fabric_blocks_table_fmac_sbus_chain,
    dnx_data_fabric_blocks_table_fsrd_ref_clk_master,

    
    _dnx_data_fabric_blocks_table_nof
} dnx_data_fabric_blocks_table_e;



const dnx_data_fabric_blocks_fsrd_sbus_chain_t * dnx_data_fabric_blocks_fsrd_sbus_chain_get(
    int unit,
    int fsrd_id);


const dnx_data_fabric_blocks_fmac_sbus_chain_t * dnx_data_fabric_blocks_fmac_sbus_chain_get(
    int unit,
    int fmac_id);


const dnx_data_fabric_blocks_fsrd_ref_clk_master_t * dnx_data_fabric_blocks_fsrd_ref_clk_master_get(
    int unit,
    int fsrd_index);



shr_error_e dnx_data_fabric_blocks_fsrd_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_blocks_fmac_sbus_chain_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_blocks_fsrd_ref_clk_master_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_blocks_fsrd_sbus_chain_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_blocks_fmac_sbus_chain_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_blocks_fsrd_ref_clk_master_info_get(
    int unit);






int dnx_data_fabric_reachability_feature_get(
    int unit,
    dnx_data_fabric_reachability_feature_e feature);



typedef enum
{
    dnx_data_fabric_reachability_define_resolution,
    dnx_data_fabric_reachability_define_gen_period,
    dnx_data_fabric_reachability_define_full_cycle_period_nsec,
    dnx_data_fabric_reachability_define_watchdog_resolution,
    dnx_data_fabric_reachability_define_watchdog_period_usec,

    
    _dnx_data_fabric_reachability_define_nof
} dnx_data_fabric_reachability_define_e;



uint32 dnx_data_fabric_reachability_resolution_get(
    int unit);


uint32 dnx_data_fabric_reachability_gen_period_get(
    int unit);


uint32 dnx_data_fabric_reachability_full_cycle_period_nsec_get(
    int unit);


uint32 dnx_data_fabric_reachability_watchdog_resolution_get(
    int unit);


uint32 dnx_data_fabric_reachability_watchdog_period_usec_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_reachability_table_nof
} dnx_data_fabric_reachability_table_e;









int dnx_data_fabric_mesh_feature_get(
    int unit,
    dnx_data_fabric_mesh_feature_e feature);



typedef enum
{
    dnx_data_fabric_mesh_define_max_mc_id,
    dnx_data_fabric_mesh_define_max_nof_dest,
    dnx_data_fabric_mesh_define_multicast_enable,
    dnx_data_fabric_mesh_define_nof_dest,

    
    _dnx_data_fabric_mesh_define_nof
} dnx_data_fabric_mesh_define_e;



uint32 dnx_data_fabric_mesh_max_mc_id_get(
    int unit);


uint32 dnx_data_fabric_mesh_max_nof_dest_get(
    int unit);


uint32 dnx_data_fabric_mesh_multicast_enable_get(
    int unit);


uint32 dnx_data_fabric_mesh_nof_dest_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_mesh_table_nof
} dnx_data_fabric_mesh_table_e;









int dnx_data_fabric_cell_feature_get(
    int unit,
    dnx_data_fabric_cell_feature_e feature);



typedef enum
{
    dnx_data_fabric_cell_define_nof_priorities,
    dnx_data_fabric_cell_define_vsc256_max_size,
    dnx_data_fabric_cell_define_max_vsc_format_size,
    dnx_data_fabric_cell_define_sr_cell_header_offset,
    dnx_data_fabric_cell_define_sr_cell_nof_instances,
    dnx_data_fabric_cell_define_sr_cell_rx_fifo_size,
    dnx_data_fabric_cell_define_sr_cell_data_valid_offset,
    dnx_data_fabric_cell_define_ctrl_cells_init_fifo_empty_iterations,
    dnx_data_fabric_cell_define_default_packing_mode,
    dnx_data_fabric_cell_define_pcp_enable,
    dnx_data_fabric_cell_define_cell_format,

    
    _dnx_data_fabric_cell_define_nof
} dnx_data_fabric_cell_define_e;



uint32 dnx_data_fabric_cell_nof_priorities_get(
    int unit);


uint32 dnx_data_fabric_cell_vsc256_max_size_get(
    int unit);


uint32 dnx_data_fabric_cell_max_vsc_format_size_get(
    int unit);


uint32 dnx_data_fabric_cell_sr_cell_header_offset_get(
    int unit);


uint32 dnx_data_fabric_cell_sr_cell_nof_instances_get(
    int unit);


uint32 dnx_data_fabric_cell_sr_cell_rx_fifo_size_get(
    int unit);


uint32 dnx_data_fabric_cell_sr_cell_data_valid_offset_get(
    int unit);


uint32 dnx_data_fabric_cell_ctrl_cells_init_fifo_empty_iterations_get(
    int unit);


uint32 dnx_data_fabric_cell_default_packing_mode_get(
    int unit);


uint32 dnx_data_fabric_cell_pcp_enable_get(
    int unit);


uint32 dnx_data_fabric_cell_cell_format_get(
    int unit);



typedef enum
{
    dnx_data_fabric_cell_table_supported_pcp_modes,
    dnx_data_fabric_cell_table_sr_cell_rx_parsing_table,

    
    _dnx_data_fabric_cell_table_nof
} dnx_data_fabric_cell_table_e;



const dnx_data_fabric_cell_supported_pcp_modes_t * dnx_data_fabric_cell_supported_pcp_modes_get(
    int unit,
    int pcp_mode);


const dnx_data_fabric_cell_sr_cell_rx_parsing_table_t * dnx_data_fabric_cell_sr_cell_rx_parsing_table_get(
    int unit,
    int index);



shr_error_e dnx_data_fabric_cell_supported_pcp_modes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_cell_sr_cell_rx_parsing_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_cell_supported_pcp_modes_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_cell_sr_cell_rx_parsing_table_info_get(
    int unit);






int dnx_data_fabric_pipes_feature_get(
    int unit,
    dnx_data_fabric_pipes_feature_e feature);



typedef enum
{
    dnx_data_fabric_pipes_define_max_nof_pipes,
    dnx_data_fabric_pipes_define_max_nof_contexts,
    dnx_data_fabric_pipes_define_max_nof_subcontexts,
    dnx_data_fabric_pipes_define_nof_pipes,
    dnx_data_fabric_pipes_define_multi_pipe_system,

    
    _dnx_data_fabric_pipes_define_nof
} dnx_data_fabric_pipes_define_e;



uint32 dnx_data_fabric_pipes_max_nof_pipes_get(
    int unit);


uint32 dnx_data_fabric_pipes_max_nof_contexts_get(
    int unit);


uint32 dnx_data_fabric_pipes_max_nof_subcontexts_get(
    int unit);


uint32 dnx_data_fabric_pipes_nof_pipes_get(
    int unit);


uint32 dnx_data_fabric_pipes_multi_pipe_system_get(
    int unit);



typedef enum
{
    dnx_data_fabric_pipes_table_map,
    dnx_data_fabric_pipes_table_valid_map_config,
    dnx_data_fabric_pipes_table_dtq_subcontexts,

    
    _dnx_data_fabric_pipes_table_nof
} dnx_data_fabric_pipes_table_e;



const dnx_data_fabric_pipes_map_t * dnx_data_fabric_pipes_map_get(
    int unit);


const dnx_data_fabric_pipes_valid_map_config_t * dnx_data_fabric_pipes_valid_map_config_get(
    int unit,
    int config_id);


const dnx_data_fabric_pipes_dtq_subcontexts_t * dnx_data_fabric_pipes_dtq_subcontexts_get(
    int unit,
    int dtq,
    int is_ocb_only);



shr_error_e dnx_data_fabric_pipes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_pipes_valid_map_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_pipes_dtq_subcontexts_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_pipes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_pipes_valid_map_config_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_pipes_dtq_subcontexts_info_get(
    int unit);






int dnx_data_fabric_debug_feature_get(
    int unit,
    dnx_data_fabric_debug_feature_e feature);



typedef enum
{
    dnx_data_fabric_debug_define_mesh_topology_size,
    dnx_data_fabric_debug_define_mesh_topology_fast,

    
    _dnx_data_fabric_debug_define_nof
} dnx_data_fabric_debug_define_e;



int dnx_data_fabric_debug_mesh_topology_size_get(
    int unit);


uint32 dnx_data_fabric_debug_mesh_topology_fast_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_debug_table_nof
} dnx_data_fabric_debug_table_e;









int dnx_data_fabric_mesh_topology_feature_get(
    int unit,
    dnx_data_fabric_mesh_topology_feature_e feature);



typedef enum
{
    dnx_data_fabric_mesh_topology_define_mesh_init,
    dnx_data_fabric_mesh_topology_define_mesh_config_1,

    
    _dnx_data_fabric_mesh_topology_define_nof
} dnx_data_fabric_mesh_topology_define_e;



uint32 dnx_data_fabric_mesh_topology_mesh_init_get(
    int unit);


uint32 dnx_data_fabric_mesh_topology_mesh_config_1_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_mesh_topology_table_nof
} dnx_data_fabric_mesh_topology_table_e;









int dnx_data_fabric_tdm_feature_get(
    int unit,
    dnx_data_fabric_tdm_feature_e feature);



typedef enum
{
    dnx_data_fabric_tdm_define_priority,

    
    _dnx_data_fabric_tdm_define_nof
} dnx_data_fabric_tdm_define_e;



int dnx_data_fabric_tdm_priority_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_tdm_table_nof
} dnx_data_fabric_tdm_table_e;









int dnx_data_fabric_cgm_feature_get(
    int unit,
    dnx_data_fabric_cgm_feature_e feature);



typedef enum
{
    dnx_data_fabric_cgm_define_nof_rci_levels,
    dnx_data_fabric_cgm_define_nof_rci_threshold_levels,
    dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution,
    dnx_data_fabric_cgm_define_egress_fifo_fc_threshold,
    dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes,
    dnx_data_fabric_cgm_define_cgm_adaptive_tfl_shaper_zone_nof,
    dnx_data_fabric_cgm_define_cgm_global_cell_shaper_max_rate,
    dnx_data_fabric_cgm_define_rci_link_level_factor_max_value,
    dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_max_threshold_value,
    dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_threshold,
    dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_threshold,
    dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_weight,
    dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_weight,
    dnx_data_fabric_cgm_define_llfc_type_default,

    
    _dnx_data_fabric_cgm_define_nof
} dnx_data_fabric_cgm_define_e;



uint32 dnx_data_fabric_cgm_nof_rci_levels_get(
    int unit);


uint32 dnx_data_fabric_cgm_nof_rci_threshold_levels_get(
    int unit);


uint32 dnx_data_fabric_cgm_egress_drop_th_clocks_resolution_get(
    int unit);


uint32 dnx_data_fabric_cgm_egress_fifo_fc_threshold_get(
    int unit);


uint32 dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_get(
    int unit);


uint32 dnx_data_fabric_cgm_cgm_adaptive_tfl_shaper_zone_nof_get(
    int unit);


uint32 dnx_data_fabric_cgm_cgm_global_cell_shaper_max_rate_get(
    int unit);


uint32 dnx_data_fabric_cgm_rci_link_level_factor_max_value_get(
    int unit);


uint32 dnx_data_fabric_cgm_local_unicast_dynamic_wfq_max_threshold_value_get(
    int unit);


uint32 dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_threshold_get(
    int unit);


uint32 dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_threshold_get(
    int unit);


uint32 dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_weight_get(
    int unit);


uint32 dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_weight_get(
    int unit);


uint32 dnx_data_fabric_cgm_llfc_type_default_get(
    int unit);



typedef enum
{
    dnx_data_fabric_cgm_table_access_map,
    dnx_data_fabric_cgm_table_ingress_dynamic_wfq_configs,

    
    _dnx_data_fabric_cgm_table_nof
} dnx_data_fabric_cgm_table_e;



const dnx_data_fabric_cgm_access_map_t * dnx_data_fabric_cgm_access_map_get(
    int unit,
    int control_type);


const dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_t * dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_get(
    int unit,
    int wfq_input);



shr_error_e dnx_data_fabric_cgm_access_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_cgm_access_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_cgm_ingress_dynamic_wfq_configs_info_get(
    int unit);






int dnx_data_fabric_dtqs_feature_get(
    int unit,
    dnx_data_fabric_dtqs_feature_e feature);



typedef enum
{
    dnx_data_fabric_dtqs_define_max_nof_subdtqs,
    dnx_data_fabric_dtqs_define_max_nof_dtqs,
    dnx_data_fabric_dtqs_define_max_size,
    dnx_data_fabric_dtqs_define_ocb_only_size,
    dnx_data_fabric_dtqs_define_nof_active_dtqs,

    
    _dnx_data_fabric_dtqs_define_nof
} dnx_data_fabric_dtqs_define_e;



uint32 dnx_data_fabric_dtqs_max_nof_subdtqs_get(
    int unit);


uint32 dnx_data_fabric_dtqs_max_nof_dtqs_get(
    int unit);


uint32 dnx_data_fabric_dtqs_max_size_get(
    int unit);


uint32 dnx_data_fabric_dtqs_ocb_only_size_get(
    int unit);


uint32 dnx_data_fabric_dtqs_nof_active_dtqs_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_dtqs_table_nof
} dnx_data_fabric_dtqs_table_e;









int dnx_data_fabric_ilkn_feature_get(
    int unit,
    dnx_data_fabric_ilkn_feature_e feature);



typedef enum
{

    
    _dnx_data_fabric_ilkn_define_nof
} dnx_data_fabric_ilkn_define_e;




typedef enum
{
    dnx_data_fabric_ilkn_table_bypass_info,

    
    _dnx_data_fabric_ilkn_table_nof
} dnx_data_fabric_ilkn_table_e;



const dnx_data_fabric_ilkn_bypass_info_t * dnx_data_fabric_ilkn_bypass_info_get(
    int unit,
    int pm_id);



shr_error_e dnx_data_fabric_ilkn_bypass_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_ilkn_bypass_info_info_get(
    int unit);






int dnx_data_fabric_dbal_feature_get(
    int unit,
    dnx_data_fabric_dbal_feature_e feature);



typedef enum
{
    dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof,
    dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof,
    dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value,
    dnx_data_fabric_dbal_define_cgm_llfc_ingress_ipt_ctx_mapping_bits_nof,
    dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof,
    dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value,
    dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof,
    dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_full_level_min_value,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_full_level_max_value,
    dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_full_level_bits_nof,
    dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof,
    dnx_data_fabric_dbal_define_cgm_egress_dynamic_wfq_th_bits_nof,
    dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value,
    dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof,
    dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof,
    dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value,
    dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof,
    dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof,
    dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof,
    dnx_data_fabric_dbal_define_adaptive_shaper_threshold_bits_nof,
    dnx_data_fabric_dbal_define_adaptive_shaper_factor_bits_nof,
    dnx_data_fabric_dbal_define_adaptive_shaper_mnol_bits_nof,
    dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_bits_nof,
    dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_granularity,
    dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_max_value,
    dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_bits_nof,
    dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_max_value,
    dnx_data_fabric_dbal_define_global_cell_shaper_cnt_bits_nof,
    dnx_data_fabric_dbal_define_fdr_links_offset,
    dnx_data_fabric_dbal_define_fabric_cgm_rci_high_severity_min_nof_links_bits_nof,
    dnx_data_fabric_dbal_define_priority_bits_nof,
    dnx_data_fabric_dbal_define_nof_links_bits_nof,

    
    _dnx_data_fabric_dbal_define_nof
} dnx_data_fabric_dbal_define_e;



uint32 dnx_data_fabric_dbal_link_rci_threshold_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_llfc_ingress_ipt_ctx_mapping_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_min_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_fifo_fc_full_level_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_egress_dynamic_wfq_th_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_adaptive_shaper_threshold_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_adaptive_shaper_factor_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_adaptive_shaper_mnol_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_granularity_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_time_frame_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_time_frame_max_value_get(
    int unit);


uint32 dnx_data_fabric_dbal_global_cell_shaper_cnt_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_fdr_links_offset_get(
    int unit);


uint32 dnx_data_fabric_dbal_fabric_cgm_rci_high_severity_min_nof_links_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_priority_bits_nof_get(
    int unit);


uint32 dnx_data_fabric_dbal_nof_links_bits_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_dbal_table_nof
} dnx_data_fabric_dbal_table_e;









int dnx_data_fabric_mesh_mc_feature_get(
    int unit,
    dnx_data_fabric_mesh_mc_feature_e feature);



typedef enum
{
    dnx_data_fabric_mesh_mc_define_nof_replication,

    
    _dnx_data_fabric_mesh_mc_define_nof
} dnx_data_fabric_mesh_mc_define_e;



uint32 dnx_data_fabric_mesh_mc_nof_replication_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_mesh_mc_table_nof
} dnx_data_fabric_mesh_mc_table_e;









int dnx_data_fabric_transmit_feature_get(
    int unit,
    dnx_data_fabric_transmit_feature_e feature);



typedef enum
{
    dnx_data_fabric_transmit_define_pdqs_total_size,
    dnx_data_fabric_transmit_define_nof_pdqs,
    dnx_data_fabric_transmit_define_s2d_pdq_size,
    dnx_data_fabric_transmit_define_pdq_sram_fc_th,
    dnx_data_fabric_transmit_define_pdq_mixs_fc_th,
    dnx_data_fabric_transmit_define_pdq_mixd_fc_th,
    dnx_data_fabric_transmit_define_pdq_mix_mixs_fc_th,
    dnx_data_fabric_transmit_define_pdq_mix_mixd_fc_th,
    dnx_data_fabric_transmit_define_pdq_ocb_fc_th,
    dnx_data_fabric_transmit_define_pdq_dram_delete_fc_th,
    dnx_data_fabric_transmit_define_pdq_sram_delete_fc_th,
    dnx_data_fabric_transmit_define_pdq_s2d_fc_th,
    dnx_data_fabric_transmit_define_pdq_sram_eir_fc_th,
    dnx_data_fabric_transmit_define_pdq_mixs_eir_fc_th,
    dnx_data_fabric_transmit_define_pdq_mixd_eir_fc_th,
    dnx_data_fabric_transmit_define_pdq_ocb_eir_fc_th,

    
    _dnx_data_fabric_transmit_define_nof
} dnx_data_fabric_transmit_define_e;



uint32 dnx_data_fabric_transmit_pdqs_total_size_get(
    int unit);


uint32 dnx_data_fabric_transmit_nof_pdqs_get(
    int unit);


uint32 dnx_data_fabric_transmit_s2d_pdq_size_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_sram_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mixs_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mixd_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mix_mixs_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mix_mixd_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_ocb_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_dram_delete_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_sram_delete_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_s2d_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_sram_eir_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mixs_eir_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_mixd_eir_fc_th_get(
    int unit);


uint32 dnx_data_fabric_transmit_pdq_ocb_eir_fc_th_get(
    int unit);



typedef enum
{
    dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select,
    dnx_data_fabric_transmit_table_eir_fc_leaky_bucket,
    dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select,
    dnx_data_fabric_transmit_table_dtq_fc_local_thresholds,
    dnx_data_fabric_transmit_table_dtq_fc_fabric_thresholds,
    dnx_data_fabric_transmit_table_dtq_fc_ocb_thresholds,
    dnx_data_fabric_transmit_table_dtq_to_pdq_ocb_thresholds,
    dnx_data_fabric_transmit_table_dtq_to_pdq_non_ocb_thresholds,
    dnx_data_fabric_transmit_table_dtq_to_pdq_fc_mapping,

    
    _dnx_data_fabric_transmit_table_nof
} dnx_data_fabric_transmit_table_e;



const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t * dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_get(
    int unit,
    int dqcq_priority);


const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t * dnx_data_fabric_transmit_eir_fc_leaky_bucket_get(
    int unit);


const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t * dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_get(
    int unit);


const dnx_data_fabric_transmit_dtq_fc_local_thresholds_t * dnx_data_fabric_transmit_dtq_fc_local_thresholds_get(
    int unit,
    int dtq_fc);


const dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_t * dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_get(
    int unit,
    int dtq_fc);


const dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_t * dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_get(
    int unit,
    int dtq_fc);


const dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_t * dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_get(
    int unit,
    int dtq_fc);


const dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_t * dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_get(
    int unit,
    int dtq_fc);


const dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_t * dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_get(
    int unit,
    int index);



shr_error_e dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_eir_fc_leaky_bucket_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_fc_local_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_eir_fc_leaky_bucket_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_fc_local_thresholds_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_fc_fabric_thresholds_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_fc_ocb_thresholds_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_to_pdq_ocb_thresholds_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_to_pdq_non_ocb_thresholds_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_fabric_transmit_dtq_to_pdq_fc_mapping_info_get(
    int unit);






int dnx_data_fabric_system_upgrade_feature_get(
    int unit,
    dnx_data_fabric_system_upgrade_feature_e feature);



typedef enum
{
    dnx_data_fabric_system_upgrade_define_max_master_id,
    dnx_data_fabric_system_upgrade_define_param_0,
    dnx_data_fabric_system_upgrade_define_param_1,
    dnx_data_fabric_system_upgrade_define_param_2,
    dnx_data_fabric_system_upgrade_define_param_3,
    dnx_data_fabric_system_upgrade_define_param_4,
    dnx_data_fabric_system_upgrade_define_param_5,
    dnx_data_fabric_system_upgrade_define_param_7,

    
    _dnx_data_fabric_system_upgrade_define_nof
} dnx_data_fabric_system_upgrade_define_e;



uint32 dnx_data_fabric_system_upgrade_max_master_id_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_0_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_1_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_2_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_3_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_4_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_5_get(
    int unit);


uint32 dnx_data_fabric_system_upgrade_param_7_get(
    int unit);



typedef enum
{
    dnx_data_fabric_system_upgrade_table_target,

    
    _dnx_data_fabric_system_upgrade_table_nof
} dnx_data_fabric_system_upgrade_table_e;



const dnx_data_fabric_system_upgrade_target_t * dnx_data_fabric_system_upgrade_target_get(
    int unit,
    int target);



shr_error_e dnx_data_fabric_system_upgrade_target_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fabric_system_upgrade_target_info_get(
    int unit);






int dnx_data_fabric_regression_feature_get(
    int unit,
    dnx_data_fabric_regression_feature_e feature);



typedef enum
{
    dnx_data_fabric_regression_define_minimum_system_ref_clk,
    dnx_data_fabric_regression_define_soft_reset_with_fabric_max_exec_time,
    dnx_data_fabric_regression_define_soft_reset_with_fabric_avg_exec_time,

    
    _dnx_data_fabric_regression_define_nof
} dnx_data_fabric_regression_define_e;



uint32 dnx_data_fabric_regression_minimum_system_ref_clk_get(
    int unit);


uint32 dnx_data_fabric_regression_soft_reset_with_fabric_max_exec_time_get(
    int unit);


uint32 dnx_data_fabric_regression_soft_reset_with_fabric_avg_exec_time_get(
    int unit);



typedef enum
{

    
    _dnx_data_fabric_regression_table_nof
} dnx_data_fabric_regression_table_e;






shr_error_e dnx_data_fabric_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

