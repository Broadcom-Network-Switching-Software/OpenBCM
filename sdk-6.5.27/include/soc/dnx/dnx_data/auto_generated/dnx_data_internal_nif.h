
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_NIF_H_

#define _DNX_DATA_INTERNAL_NIF_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_nif_submodule_global,
    dnx_data_nif_submodule_phys,
    dnx_data_nif_submodule_ilkn,
    dnx_data_nif_submodule_eth,
    dnx_data_nif_submodule_simulator,
    dnx_data_nif_submodule_framer,
    dnx_data_nif_submodule_mac_client,
    dnx_data_nif_submodule_sar_client,
    dnx_data_nif_submodule_flexe,
    dnx_data_nif_submodule_wbc,
    dnx_data_nif_submodule_prd,
    dnx_data_nif_submodule_portmod,
    dnx_data_nif_submodule_scheduler,
    dnx_data_nif_submodule_dbal,
    dnx_data_nif_submodule_features,
    dnx_data_nif_submodule_arb,
    dnx_data_nif_submodule_ofr,
    dnx_data_nif_submodule_oft,
    dnx_data_nif_submodule_signal_quality,
    dnx_data_nif_submodule_otn,
    dnx_data_nif_submodule_flr,

    
    _dnx_data_nif_submodule_nof
} dnx_data_nif_submodule_e;








int dnx_data_nif_global_feature_get(
    int unit,
    dnx_data_nif_global_feature_e feature);



typedef enum
{
    dnx_data_nif_global_define_nof_lcplls,
    dnx_data_nif_global_define_nof_nif_interfaces_per_core,
    dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity,
    dnx_data_nif_global_define_nof_nif_units_per_core,
    dnx_data_nif_global_define_reassembler_fifo_threshold,
    dnx_data_nif_global_define_total_data_capacity,
    dnx_data_nif_global_define_single_mgmt_port_capacity,
    dnx_data_nif_global_define_max_nof_lanes_per_port,
    dnx_data_nif_global_define_ghao_speed_limit,
    dnx_data_nif_global_define_start_tx_threshold_global,
    dnx_data_nif_global_define_l1_only_mode,
    dnx_data_nif_global_define_otn_supported,

    
    _dnx_data_nif_global_define_nof
} dnx_data_nif_global_define_e;



uint32 dnx_data_nif_global_nof_lcplls_get(
    int unit);


uint32 dnx_data_nif_global_nof_nif_interfaces_per_core_get(
    int unit);


uint32 dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get(
    int unit);


uint32 dnx_data_nif_global_nof_nif_units_per_core_get(
    int unit);


uint32 dnx_data_nif_global_reassembler_fifo_threshold_get(
    int unit);


uint32 dnx_data_nif_global_total_data_capacity_get(
    int unit);


uint32 dnx_data_nif_global_single_mgmt_port_capacity_get(
    int unit);


uint32 dnx_data_nif_global_max_nof_lanes_per_port_get(
    int unit);


uint32 dnx_data_nif_global_ghao_speed_limit_get(
    int unit);


uint32 dnx_data_nif_global_start_tx_threshold_global_get(
    int unit);


uint32 dnx_data_nif_global_l1_only_mode_get(
    int unit);


uint32 dnx_data_nif_global_otn_supported_get(
    int unit);



typedef enum
{
    dnx_data_nif_global_table_pll_phys,
    dnx_data_nif_global_table_nif_interface_id_to_unit_id,

    
    _dnx_data_nif_global_table_nof
} dnx_data_nif_global_table_e;



const dnx_data_nif_global_pll_phys_t * dnx_data_nif_global_pll_phys_get(
    int unit,
    int lcpll);


const dnx_data_nif_global_nif_interface_id_to_unit_id_t * dnx_data_nif_global_nif_interface_id_to_unit_id_get(
    int unit,
    int nif_interface_id);



shr_error_e dnx_data_nif_global_pll_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_global_nif_interface_id_to_unit_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_global_pll_phys_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_global_nif_interface_id_to_unit_id_info_get(
    int unit);






int dnx_data_nif_phys_feature_get(
    int unit,
    dnx_data_nif_phys_feature_e feature);



typedef enum
{
    dnx_data_nif_phys_define_nof_phys,
    dnx_data_nif_phys_define_max_phys_in_core,
    dnx_data_nif_phys_define_pm8x50_gen,
    dnx_data_nif_phys_define_is_pam4_speed_supported,
    dnx_data_nif_phys_define_first_mgmt_phy,
    dnx_data_nif_phys_define_nof_mgmt_phys,
    dnx_data_nif_phys_define_txpi_sdm_scheme,

    
    _dnx_data_nif_phys_define_nof
} dnx_data_nif_phys_define_e;



uint32 dnx_data_nif_phys_nof_phys_get(
    int unit);


uint32 dnx_data_nif_phys_max_phys_in_core_get(
    int unit);


uint32 dnx_data_nif_phys_pm8x50_gen_get(
    int unit);


uint32 dnx_data_nif_phys_is_pam4_speed_supported_get(
    int unit);


uint32 dnx_data_nif_phys_first_mgmt_phy_get(
    int unit);


uint32 dnx_data_nif_phys_nof_mgmt_phys_get(
    int unit);


uint32 dnx_data_nif_phys_txpi_sdm_scheme_get(
    int unit);



typedef enum
{
    dnx_data_nif_phys_table_general,
    dnx_data_nif_phys_table_polarity,
    dnx_data_nif_phys_table_core_phys_map,
    dnx_data_nif_phys_table_vco_div,
    dnx_data_nif_phys_table_nof_phys_per_core,
    dnx_data_nif_phys_table_txpi_sdm_div,

    
    _dnx_data_nif_phys_table_nof
} dnx_data_nif_phys_table_e;



const dnx_data_nif_phys_general_t * dnx_data_nif_phys_general_get(
    int unit);


const dnx_data_nif_phys_polarity_t * dnx_data_nif_phys_polarity_get(
    int unit,
    int lane_index);


const dnx_data_nif_phys_core_phys_map_t * dnx_data_nif_phys_core_phys_map_get(
    int unit,
    int core_index);


const dnx_data_nif_phys_vco_div_t * dnx_data_nif_phys_vco_div_get(
    int unit,
    int ethu_index);


const dnx_data_nif_phys_nof_phys_per_core_t * dnx_data_nif_phys_nof_phys_per_core_get(
    int unit,
    int core_index);


const dnx_data_nif_phys_txpi_sdm_div_t * dnx_data_nif_phys_txpi_sdm_div_get(
    int unit,
    int index);



shr_error_e dnx_data_nif_phys_general_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_phys_polarity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_phys_core_phys_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_phys_vco_div_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_phys_nof_phys_per_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_phys_txpi_sdm_div_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_phys_general_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_polarity_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_core_phys_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_vco_div_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_nof_phys_per_core_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_txpi_sdm_div_info_get(
    int unit);






int dnx_data_nif_ilkn_feature_get(
    int unit,
    dnx_data_nif_ilkn_feature_e feature);



typedef enum
{
    dnx_data_nif_ilkn_define_ilu_nof,
    dnx_data_nif_ilkn_define_ilkn_unit_nof,
    dnx_data_nif_ilkn_define_ilkn_unit_if_nof,
    dnx_data_nif_ilkn_define_ilkn_if_nof,
    dnx_data_nif_ilkn_define_fec_units_per_core_nof,
    dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit,
    dnx_data_nif_ilkn_define_lanes_max_nof,
    dnx_data_nif_ilkn_define_lanes_max_nof_using_fec,
    dnx_data_nif_ilkn_define_lanes_allowed_nof,
    dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max,
    dnx_data_nif_ilkn_define_segments_max_nof,
    dnx_data_nif_ilkn_define_segments_half_nof,
    dnx_data_nif_ilkn_define_pms_nof,
    dnx_data_nif_ilkn_define_fmac_bus_size,
    dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof,
    dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof,
    dnx_data_nif_ilkn_define_data_rx_hrf_size,
    dnx_data_nif_ilkn_define_tdm_rx_hrf_size,
    dnx_data_nif_ilkn_define_tx_hrf_credits,
    dnx_data_nif_ilkn_define_nof_rx_hrf_per_port,
    dnx_data_nif_ilkn_define_watermark_high_elk,
    dnx_data_nif_ilkn_define_watermark_low_elk,
    dnx_data_nif_ilkn_define_watermark_high_data,
    dnx_data_nif_ilkn_define_watermark_low_data,
    dnx_data_nif_ilkn_define_pad_size,
    dnx_data_nif_ilkn_define_burst_max_range_max,
    dnx_data_nif_ilkn_define_burst_min,
    dnx_data_nif_ilkn_define_burst_short,
    dnx_data_nif_ilkn_define_burst_min_range_max,
    dnx_data_nif_ilkn_define_max_nof_ifs,
    dnx_data_nif_ilkn_define_max_nof_elk_ifs,
    dnx_data_nif_ilkn_define_port_0_status_intr_id,
    dnx_data_nif_ilkn_define_port_1_status_intr_id,
    dnx_data_nif_ilkn_define_link_stable_wait,
    dnx_data_nif_ilkn_define_interleaved_error_drop_single_context,

    
    _dnx_data_nif_ilkn_define_nof
} dnx_data_nif_ilkn_define_e;



uint32 dnx_data_nif_ilkn_ilu_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_unit_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_unit_if_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_if_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_fec_units_per_core_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_nof_lanes_per_fec_unit_get(
    int unit);


uint32 dnx_data_nif_ilkn_lanes_max_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_lanes_max_nof_using_fec_get(
    int unit);


uint32 dnx_data_nif_ilkn_lanes_allowed_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_over_eth_pms_max_get(
    int unit);


uint32 dnx_data_nif_ilkn_segments_max_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_segments_half_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_pms_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_fmac_bus_size_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_rx_hrf_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_ilkn_tx_hrf_nof_get(
    int unit);


uint32 dnx_data_nif_ilkn_data_rx_hrf_size_get(
    int unit);


uint32 dnx_data_nif_ilkn_tdm_rx_hrf_size_get(
    int unit);


uint32 dnx_data_nif_ilkn_tx_hrf_credits_get(
    int unit);


uint32 dnx_data_nif_ilkn_nof_rx_hrf_per_port_get(
    int unit);


uint32 dnx_data_nif_ilkn_watermark_high_elk_get(
    int unit);


uint32 dnx_data_nif_ilkn_watermark_low_elk_get(
    int unit);


uint32 dnx_data_nif_ilkn_watermark_high_data_get(
    int unit);


uint32 dnx_data_nif_ilkn_watermark_low_data_get(
    int unit);


uint32 dnx_data_nif_ilkn_pad_size_get(
    int unit);


uint32 dnx_data_nif_ilkn_burst_max_range_max_get(
    int unit);


uint32 dnx_data_nif_ilkn_burst_min_get(
    int unit);


uint32 dnx_data_nif_ilkn_burst_short_get(
    int unit);


uint32 dnx_data_nif_ilkn_burst_min_range_max_get(
    int unit);


uint32 dnx_data_nif_ilkn_max_nof_ifs_get(
    int unit);


uint32 dnx_data_nif_ilkn_max_nof_elk_ifs_get(
    int unit);


uint32 dnx_data_nif_ilkn_port_0_status_intr_id_get(
    int unit);


uint32 dnx_data_nif_ilkn_port_1_status_intr_id_get(
    int unit);


uint32 dnx_data_nif_ilkn_link_stable_wait_get(
    int unit);


uint32 dnx_data_nif_ilkn_interleaved_error_drop_single_context_get(
    int unit);



typedef enum
{
    dnx_data_nif_ilkn_table_phys,
    dnx_data_nif_ilkn_table_supported_phys,
    dnx_data_nif_ilkn_table_supported_interfaces,
    dnx_data_nif_ilkn_table_ilkn_cores,
    dnx_data_nif_ilkn_table_ilkn_cores_clup_facing,
    dnx_data_nif_ilkn_table_supported_device_core,
    dnx_data_nif_ilkn_table_properties,
    dnx_data_nif_ilkn_table_nif_pms,
    dnx_data_nif_ilkn_table_fabric_pms,
    dnx_data_nif_ilkn_table_nif_lanes_map,
    dnx_data_nif_ilkn_table_fabric_lanes_map,
    dnx_data_nif_ilkn_table_start_tx_threshold_table,
    dnx_data_nif_ilkn_table_connectivity_options,

    
    _dnx_data_nif_ilkn_table_nof
} dnx_data_nif_ilkn_table_e;



const dnx_data_nif_ilkn_phys_t * dnx_data_nif_ilkn_phys_get(
    int unit,
    int ilkn_id);


const dnx_data_nif_ilkn_supported_phys_t * dnx_data_nif_ilkn_supported_phys_get(
    int unit,
    int ilkn_id);


const dnx_data_nif_ilkn_supported_interfaces_t * dnx_data_nif_ilkn_supported_interfaces_get(
    int unit,
    int index);


const dnx_data_nif_ilkn_ilkn_cores_t * dnx_data_nif_ilkn_ilkn_cores_get(
    int unit,
    int ilkn_core_id);


const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t * dnx_data_nif_ilkn_ilkn_cores_clup_facing_get(
    int unit,
    int ilkn_core_id);


const dnx_data_nif_ilkn_supported_device_core_t * dnx_data_nif_ilkn_supported_device_core_get(
    int unit,
    int ilkn_id);


const dnx_data_nif_ilkn_properties_t * dnx_data_nif_ilkn_properties_get(
    int unit,
    int ilkn_id);


const dnx_data_nif_ilkn_nif_pms_t * dnx_data_nif_ilkn_nif_pms_get(
    int unit,
    int pm_id);


const dnx_data_nif_ilkn_fabric_pms_t * dnx_data_nif_ilkn_fabric_pms_get(
    int unit,
    int pm_id);


const dnx_data_nif_ilkn_nif_lanes_map_t * dnx_data_nif_ilkn_nif_lanes_map_get(
    int unit,
    int ilkn_core,
    int range_id);


const dnx_data_nif_ilkn_fabric_lanes_map_t * dnx_data_nif_ilkn_fabric_lanes_map_get(
    int unit,
    int ilkn_core,
    int range_id);


const dnx_data_nif_ilkn_start_tx_threshold_table_t * dnx_data_nif_ilkn_start_tx_threshold_table_get(
    int unit,
    int idx);


const dnx_data_nif_ilkn_connectivity_options_t * dnx_data_nif_ilkn_connectivity_options_get(
    int unit,
    int ilkn_id,
    int connectivity_mode);



shr_error_e dnx_data_nif_ilkn_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_supported_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_ilkn_cores_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_ilkn_cores_clup_facing_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_supported_device_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_nif_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_fabric_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_nif_lanes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_fabric_lanes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_ilkn_connectivity_options_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_ilkn_phys_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_supported_phys_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_supported_interfaces_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_ilkn_cores_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_ilkn_cores_clup_facing_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_supported_device_core_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_properties_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_nif_pms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_fabric_pms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_nif_lanes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_fabric_lanes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_start_tx_threshold_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_connectivity_options_info_get(
    int unit);






int dnx_data_nif_eth_feature_get(
    int unit,
    dnx_data_nif_eth_feature_e feature);



typedef enum
{
    dnx_data_nif_eth_define_cdu_nof,
    dnx_data_nif_eth_define_clu_nof,
    dnx_data_nif_eth_define_nbu_nof,
    dnx_data_nif_eth_define_mgu_nof,
    dnx_data_nif_eth_define_cdum_nof,
    dnx_data_nif_eth_define_nof_pms_in_cdu,
    dnx_data_nif_eth_define_nof_pms_in_clu,
    dnx_data_nif_eth_define_nof_pms_in_nbu,
    dnx_data_nif_eth_define_nof_pms_in_mgu,
    dnx_data_nif_eth_define_total_nof_ethu_pms_in_device,
    dnx_data_nif_eth_define_nof_cdu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_clu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_cdu_pms,
    dnx_data_nif_eth_define_nof_clu_pms,
    dnx_data_nif_eth_define_nof_nbu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_mgu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_nbu_pms,
    dnx_data_nif_eth_define_nof_mgu_pms,
    dnx_data_nif_eth_define_ethu_nof,
    dnx_data_nif_eth_define_cdu_nof_per_core,
    dnx_data_nif_eth_define_clu_nof_per_core,
    dnx_data_nif_eth_define_max_ethu_nof_per_core,
    dnx_data_nif_eth_define_nof_lanes_in_cdu,
    dnx_data_nif_eth_define_nof_lanes_in_clu,
    dnx_data_nif_eth_define_nof_lanes_in_nbu,
    dnx_data_nif_eth_define_nof_lanes_in_mgu,
    dnx_data_nif_eth_define_max_nof_lanes_in_ethu,
    dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device,
    dnx_data_nif_eth_define_total_nof_nbu_lanes_in_device,
    dnx_data_nif_eth_define_cdu_mac_mode_config_nof,
    dnx_data_nif_eth_define_nbu_mac_mode_config_nof,
    dnx_data_nif_eth_define_mac_mode_config_lanes_nof,
    dnx_data_nif_eth_define_cdu_mac_nof,
    dnx_data_nif_eth_define_nbu_mac_nof,
    dnx_data_nif_eth_define_mac_lanes_nof,
    dnx_data_nif_eth_define_ethu_logical_fifo_nof,
    dnx_data_nif_eth_define_cdu_memory_entries_nof,
    dnx_data_nif_eth_define_clu_memory_entries_nof,
    dnx_data_nif_eth_define_nbu_memory_entries_nof,
    dnx_data_nif_eth_define_mgu_memory_entries_nof,
    dnx_data_nif_eth_define_priority_group_nof_entries_min,
    dnx_data_nif_eth_define_priority_groups_nof,
    dnx_data_nif_eth_define_nof_rmcs_per_priority_group,
    dnx_data_nif_eth_define_pad_size_min,
    dnx_data_nif_eth_define_pad_size_max,
    dnx_data_nif_eth_define_packet_size_max,
    dnx_data_nif_eth_define_an_max_nof_abilities,
    dnx_data_nif_eth_define_phy_map_granularity,
    dnx_data_nif_eth_define_vco_factor_for_53p125_rate,
    dnx_data_nif_eth_define_vco_factor_for_51p5625_rate,
    dnx_data_nif_eth_define_is_400G_supported,
    dnx_data_nif_eth_define_is_800G_supported,

    
    _dnx_data_nif_eth_define_nof
} dnx_data_nif_eth_define_e;



uint32 dnx_data_nif_eth_cdu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_clu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nbu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mgu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdum_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_cdu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_clu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_nbu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_mgu_get(
    int unit);


uint32 dnx_data_nif_eth_total_nof_ethu_pms_in_device_get(
    int unit);


uint32 dnx_data_nif_eth_nof_cdu_lanes_in_pm_get(
    int unit);


uint32 dnx_data_nif_eth_nof_clu_lanes_in_pm_get(
    int unit);


uint32 dnx_data_nif_eth_nof_cdu_pms_get(
    int unit);


uint32 dnx_data_nif_eth_nof_clu_pms_get(
    int unit);


uint32 dnx_data_nif_eth_nof_nbu_lanes_in_pm_get(
    int unit);


uint32 dnx_data_nif_eth_nof_mgu_lanes_in_pm_get(
    int unit);


uint32 dnx_data_nif_eth_nof_nbu_pms_get(
    int unit);


uint32 dnx_data_nif_eth_nof_mgu_pms_get(
    int unit);


uint32 dnx_data_nif_eth_ethu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_clu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_max_ethu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_cdu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_clu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_nbu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_mgu_get(
    int unit);


uint32 dnx_data_nif_eth_max_nof_lanes_in_ethu_get(
    int unit);


uint32 dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get(
    int unit);


uint32 dnx_data_nif_eth_total_nof_nbu_lanes_in_device_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_mac_mode_config_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nbu_mac_mode_config_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mac_mode_config_lanes_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_mac_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nbu_mac_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mac_lanes_nof_get(
    int unit);


uint32 dnx_data_nif_eth_ethu_logical_fifo_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_clu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nbu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mgu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_priority_group_nof_entries_min_get(
    int unit);


uint32 dnx_data_nif_eth_priority_groups_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nof_rmcs_per_priority_group_get(
    int unit);


uint32 dnx_data_nif_eth_pad_size_min_get(
    int unit);


uint32 dnx_data_nif_eth_pad_size_max_get(
    int unit);


uint32 dnx_data_nif_eth_packet_size_max_get(
    int unit);


uint32 dnx_data_nif_eth_an_max_nof_abilities_get(
    int unit);


uint32 dnx_data_nif_eth_phy_map_granularity_get(
    int unit);


uint32 dnx_data_nif_eth_vco_factor_for_53p125_rate_get(
    int unit);


uint32 dnx_data_nif_eth_vco_factor_for_51p5625_rate_get(
    int unit);


uint32 dnx_data_nif_eth_is_400G_supported_get(
    int unit);


uint32 dnx_data_nif_eth_is_800G_supported_get(
    int unit);



typedef enum
{
    dnx_data_nif_eth_table_start_tx_threshold_table,
    dnx_data_nif_eth_table_pm_properties,
    dnx_data_nif_eth_table_ethu_properties,
    dnx_data_nif_eth_table_phy_map,
    dnx_data_nif_eth_table_is_nif_loopback_supported,
    dnx_data_nif_eth_table_mac_padding,
    dnx_data_nif_eth_table_max_speed,
    dnx_data_nif_eth_table_supported_interfaces,
    dnx_data_nif_eth_table_ethu_per_core,
    dnx_data_nif_eth_table_nif_cores_ethus,
    dnx_data_nif_eth_table_supported_clu_mgu_an_abilities,
    dnx_data_nif_eth_table_ingress_reassembly_context_mapping,

    
    _dnx_data_nif_eth_table_nof
} dnx_data_nif_eth_table_e;



const dnx_data_nif_eth_start_tx_threshold_table_t * dnx_data_nif_eth_start_tx_threshold_table_get(
    int unit,
    int idx);


const dnx_data_nif_eth_pm_properties_t * dnx_data_nif_eth_pm_properties_get(
    int unit,
    int pm_index);


const dnx_data_nif_eth_ethu_properties_t * dnx_data_nif_eth_ethu_properties_get(
    int unit,
    int ethu_index);


const dnx_data_nif_eth_phy_map_t * dnx_data_nif_eth_phy_map_get(
    int unit,
    int idx);


const dnx_data_nif_eth_is_nif_loopback_supported_t * dnx_data_nif_eth_is_nif_loopback_supported_get(
    int unit,
    int type);


const dnx_data_nif_eth_mac_padding_t * dnx_data_nif_eth_mac_padding_get(
    int unit,
    int type);


const dnx_data_nif_eth_max_speed_t * dnx_data_nif_eth_max_speed_get(
    int unit,
    int lane_num);


const dnx_data_nif_eth_supported_interfaces_t * dnx_data_nif_eth_supported_interfaces_get(
    int unit,
    int idx);


const dnx_data_nif_eth_ethu_per_core_t * dnx_data_nif_eth_ethu_per_core_get(
    int unit,
    int ethu_index);


const dnx_data_nif_eth_nif_cores_ethus_t * dnx_data_nif_eth_nif_cores_ethus_get(
    int unit,
    int core_id);


const dnx_data_nif_eth_supported_clu_mgu_an_abilities_t * dnx_data_nif_eth_supported_clu_mgu_an_abilities_get(
    int unit,
    int idx);


const dnx_data_nif_eth_ingress_reassembly_context_mapping_t * dnx_data_nif_eth_ingress_reassembly_context_mapping_get(
    int unit,
    int scheduler_priority);



shr_error_e dnx_data_nif_eth_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_pm_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_ethu_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_phy_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_is_nif_loopback_supported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_mac_padding_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_max_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_ethu_per_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_nif_cores_ethus_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_supported_clu_mgu_an_abilities_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_eth_ingress_reassembly_context_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_eth_start_tx_threshold_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_pm_properties_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_ethu_properties_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_phy_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_is_nif_loopback_supported_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_mac_padding_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_max_speed_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_supported_interfaces_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_ethu_per_core_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_nif_cores_ethus_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_supported_clu_mgu_an_abilities_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_ingress_reassembly_context_mapping_info_get(
    int unit);






int dnx_data_nif_simulator_feature_get(
    int unit,
    dnx_data_nif_simulator_feature_e feature);



typedef enum
{
    dnx_data_nif_simulator_define_cdu_type,
    dnx_data_nif_simulator_define_clu_type,
    dnx_data_nif_simulator_define_nbu_type,
    dnx_data_nif_simulator_define_mgu_type,

    
    _dnx_data_nif_simulator_define_nof
} dnx_data_nif_simulator_define_e;



uint32 dnx_data_nif_simulator_cdu_type_get(
    int unit);


uint32 dnx_data_nif_simulator_clu_type_get(
    int unit);


uint32 dnx_data_nif_simulator_nbu_type_get(
    int unit);


uint32 dnx_data_nif_simulator_mgu_type_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_simulator_table_nof
} dnx_data_nif_simulator_table_e;









int dnx_data_nif_framer_feature_get(
    int unit,
    dnx_data_nif_framer_feature_e feature);



typedef enum
{
    dnx_data_nif_framer_define_nof_units,
    dnx_data_nif_framer_define_b66switch_backup_channels,
    dnx_data_nif_framer_define_gen,
    dnx_data_nif_framer_define_nof_mgmt_channels,
    dnx_data_nif_framer_define_clock_mhz,
    dnx_data_nif_framer_define_ptp_channel_max_bandwidth,
    dnx_data_nif_framer_define_oam_channel_max_bandwidth,
    dnx_data_nif_framer_define_nof_mac1_channels,
    dnx_data_nif_framer_define_nof_mac2_channels,
    dnx_data_nif_framer_define_nof_pms_per_framer,

    
    _dnx_data_nif_framer_define_nof
} dnx_data_nif_framer_define_e;



uint32 dnx_data_nif_framer_nof_units_get(
    int unit);


uint32 dnx_data_nif_framer_b66switch_backup_channels_get(
    int unit);


uint32 dnx_data_nif_framer_gen_get(
    int unit);


uint32 dnx_data_nif_framer_nof_mgmt_channels_get(
    int unit);


uint32 dnx_data_nif_framer_clock_mhz_get(
    int unit);


uint32 dnx_data_nif_framer_ptp_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_framer_oam_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_framer_nof_mac1_channels_get(
    int unit);


uint32 dnx_data_nif_framer_nof_mac2_channels_get(
    int unit);


uint32 dnx_data_nif_framer_nof_pms_per_framer_get(
    int unit);



typedef enum
{
    dnx_data_nif_framer_table_b66switch,
    dnx_data_nif_framer_table_mac_info,
    dnx_data_nif_framer_table_mgmt_channel_info,

    
    _dnx_data_nif_framer_table_nof
} dnx_data_nif_framer_table_e;



const dnx_data_nif_framer_b66switch_t * dnx_data_nif_framer_b66switch_get(
    int unit,
    int port_type);


const dnx_data_nif_framer_mac_info_t * dnx_data_nif_framer_mac_info_get(
    int unit,
    int index);


const dnx_data_nif_framer_mgmt_channel_info_t * dnx_data_nif_framer_mgmt_channel_info_get(
    int unit,
    int channel);



shr_error_e dnx_data_nif_framer_b66switch_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_framer_mac_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_framer_mgmt_channel_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_framer_b66switch_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_framer_mac_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_framer_mgmt_channel_info_info_get(
    int unit);






int dnx_data_nif_mac_client_feature_get(
    int unit,
    dnx_data_nif_mac_client_feature_e feature);



typedef enum
{
    dnx_data_nif_mac_client_define_nof_normal_clients,
    dnx_data_nif_mac_client_define_nof_special_clients,
    dnx_data_nif_mac_client_define_nof_clients,
    dnx_data_nif_mac_client_define_channel_base,
    dnx_data_nif_mac_client_define_max_nof_slots,
    dnx_data_nif_mac_client_define_nof_logical_fifos,
    dnx_data_nif_mac_client_define_oam_client_channel,
    dnx_data_nif_mac_client_define_ptp_client_channel,
    dnx_data_nif_mac_client_define_oam_tx_cal_slot,
    dnx_data_nif_mac_client_define_ptp_tx_cal_slot,
    dnx_data_nif_mac_client_define_priority_groups_nof,
    dnx_data_nif_mac_client_define_ptp_channel_max_bandwidth,
    dnx_data_nif_mac_client_define_oam_channel_max_bandwidth,
    dnx_data_nif_mac_client_define_nof_mem_entries_per_slot,
    dnx_data_nif_mac_client_define_ptp_oam_fifo_entries_in_sb_rx,
    dnx_data_nif_mac_client_define_average_ipg_for_l1,
    dnx_data_nif_mac_client_define_mac_tx_threshold,
    dnx_data_nif_mac_client_define_rmc_cal_nof_slots,
    dnx_data_nif_mac_client_define_nof_mac_timeslots,
    dnx_data_nif_mac_client_define_tx_average_ipg_max,
    dnx_data_nif_mac_client_define_tx_threshold_max,
    dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max,
    dnx_data_nif_mac_client_define_sb_fifo_granularity,
    dnx_data_nif_mac_client_define_sb_cal_granularity,
    dnx_data_nif_mac_client_define_nof_sb_fifos,
    dnx_data_nif_mac_client_define_nof_sb_cal_slots,
    dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo,
    dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo,
    dnx_data_nif_mac_client_define_tinymac_clock_mhz,
    dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g,
    dnx_data_nif_mac_client_define_async_fifo_delay_cycles,
    dnx_data_nif_mac_client_define_pipe_delay_cycles,
    dnx_data_nif_mac_client_define_mac_pad_size,

    
    _dnx_data_nif_mac_client_define_nof
} dnx_data_nif_mac_client_define_e;



uint32 dnx_data_nif_mac_client_nof_normal_clients_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_special_clients_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_clients_get(
    int unit);


uint32 dnx_data_nif_mac_client_channel_base_get(
    int unit);


uint32 dnx_data_nif_mac_client_max_nof_slots_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_logical_fifos_get(
    int unit);


uint32 dnx_data_nif_mac_client_oam_client_channel_get(
    int unit);


uint32 dnx_data_nif_mac_client_ptp_client_channel_get(
    int unit);


uint32 dnx_data_nif_mac_client_oam_tx_cal_slot_get(
    int unit);


uint32 dnx_data_nif_mac_client_ptp_tx_cal_slot_get(
    int unit);


uint32 dnx_data_nif_mac_client_priority_groups_nof_get(
    int unit);


uint32 dnx_data_nif_mac_client_ptp_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_mac_client_oam_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_mem_entries_per_slot_get(
    int unit);


uint32 dnx_data_nif_mac_client_ptp_oam_fifo_entries_in_sb_rx_get(
    int unit);


uint32 dnx_data_nif_mac_client_average_ipg_for_l1_get(
    int unit);


uint32 dnx_data_nif_mac_client_mac_tx_threshold_get(
    int unit);


uint32 dnx_data_nif_mac_client_rmc_cal_nof_slots_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_mac_timeslots_get(
    int unit);


uint32 dnx_data_nif_mac_client_tx_average_ipg_max_get(
    int unit);


uint32 dnx_data_nif_mac_client_tx_threshold_max_get(
    int unit);


uint32 dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_get(
    int unit);


uint32 dnx_data_nif_mac_client_sb_fifo_granularity_get(
    int unit);


uint32 dnx_data_nif_mac_client_sb_cal_granularity_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_sb_fifos_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_sb_cal_slots_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_get(
    int unit);


uint32 dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_get(
    int unit);


uint32 dnx_data_nif_mac_client_tinymac_clock_mhz_get(
    int unit);


uint32 dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_get(
    int unit);


uint32 dnx_data_nif_mac_client_async_fifo_delay_cycles_get(
    int unit);


uint32 dnx_data_nif_mac_client_pipe_delay_cycles_get(
    int unit);


uint32 dnx_data_nif_mac_client_mac_pad_size_get(
    int unit);



typedef enum
{
    dnx_data_nif_mac_client_table_start_tx_threshold_table,
    dnx_data_nif_mac_client_table_l1_mismatch_rate_table,
    dnx_data_nif_mac_client_table_speed_tx_property_table,

    
    _dnx_data_nif_mac_client_table_nof
} dnx_data_nif_mac_client_table_e;



const dnx_data_nif_mac_client_start_tx_threshold_table_t * dnx_data_nif_mac_client_start_tx_threshold_table_get(
    int unit,
    int idx);


const dnx_data_nif_mac_client_l1_mismatch_rate_table_t * dnx_data_nif_mac_client_l1_mismatch_rate_table_get(
    int unit,
    int idx);


const dnx_data_nif_mac_client_speed_tx_property_table_t * dnx_data_nif_mac_client_speed_tx_property_table_get(
    int unit,
    int idx);



shr_error_e dnx_data_nif_mac_client_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_mac_client_l1_mismatch_rate_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_mac_client_speed_tx_property_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_mac_client_start_tx_threshold_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_mac_client_l1_mismatch_rate_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_mac_client_speed_tx_property_table_info_get(
    int unit);






int dnx_data_nif_sar_client_feature_get(
    int unit,
    dnx_data_nif_sar_client_feature_e feature);



typedef enum
{
    dnx_data_nif_sar_client_define_nof_clients,
    dnx_data_nif_sar_client_define_ilkn_burst_size_in_feu,
    dnx_data_nif_sar_client_define_nof_sar_timeslots,
    dnx_data_nif_sar_client_define_channel_base,
    dnx_data_nif_sar_client_define_sar_jitter_tolerance_max_level,
    dnx_data_nif_sar_client_define_cal_granularity,

    
    _dnx_data_nif_sar_client_define_nof
} dnx_data_nif_sar_client_define_e;



uint32 dnx_data_nif_sar_client_nof_clients_get(
    int unit);


uint32 dnx_data_nif_sar_client_ilkn_burst_size_in_feu_get(
    int unit);


uint32 dnx_data_nif_sar_client_nof_sar_timeslots_get(
    int unit);


uint32 dnx_data_nif_sar_client_channel_base_get(
    int unit);


uint32 dnx_data_nif_sar_client_sar_jitter_tolerance_max_level_get(
    int unit);


uint32 dnx_data_nif_sar_client_cal_granularity_get(
    int unit);



typedef enum
{
    dnx_data_nif_sar_client_table_sar_cell_mode_to_size,
    dnx_data_nif_sar_client_table_sar_jitter_fifo_levels,

    
    _dnx_data_nif_sar_client_table_nof
} dnx_data_nif_sar_client_table_e;



const dnx_data_nif_sar_client_sar_cell_mode_to_size_t * dnx_data_nif_sar_client_sar_cell_mode_to_size_get(
    int unit,
    int cell_mode);


const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t * dnx_data_nif_sar_client_sar_jitter_fifo_levels_get(
    int unit,
    int tolerance_level);



shr_error_e dnx_data_nif_sar_client_sar_cell_mode_to_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_sar_client_sar_jitter_fifo_levels_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_sar_client_sar_cell_mode_to_size_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_sar_client_sar_jitter_fifo_levels_info_get(
    int unit);






int dnx_data_nif_flexe_feature_get(
    int unit,
    dnx_data_nif_flexe_feature_e feature);



typedef enum
{
    dnx_data_nif_flexe_define_feu_nof,
    dnx_data_nif_flexe_define_nof_clients,
    dnx_data_nif_flexe_define_nof_groups,
    dnx_data_nif_flexe_define_max_group_id,
    dnx_data_nif_flexe_define_min_group_id,
    dnx_data_nif_flexe_define_nb_tx_delay,
    dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read,
    dnx_data_nif_flexe_define_nof_pcs,
    dnx_data_nif_flexe_define_nof_phy_speeds,
    dnx_data_nif_flexe_define_phy_speed_granularity,
    dnx_data_nif_flexe_define_max_flexe_core_speed,
    dnx_data_nif_flexe_define_flexe_core_clock_source_pm,
    dnx_data_nif_flexe_define_max_nof_slots,
    dnx_data_nif_flexe_define_min_client_id,
    dnx_data_nif_flexe_define_max_client_id,
    dnx_data_nif_flexe_define_nof_flexe_instances,
    dnx_data_nif_flexe_define_nof_flexe_lphys,
    dnx_data_nif_flexe_define_distributed_ilu_id,
    dnx_data_nif_flexe_define_max_nif_rate_centralized,
    dnx_data_nif_flexe_define_max_nif_rate_distributed,
    dnx_data_nif_flexe_define_max_ilu_rate_distributed,
    dnx_data_nif_flexe_define_nof_flexe_core_ports,
    dnx_data_nif_flexe_define_nb_cal_nof_slots,
    dnx_data_nif_flexe_define_nb_cal_granularity,
    dnx_data_nif_flexe_define_non_5g_slot_bw,
    dnx_data_nif_flexe_define_nb_cal_slot_nof_bits,
    dnx_data_nif_flexe_define_flexe_mode,
    dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode,
    dnx_data_nif_flexe_define_device_slot_mode,

    
    _dnx_data_nif_flexe_define_nof
} dnx_data_nif_flexe_define_e;



uint32 dnx_data_nif_flexe_feu_nof_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_clients_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_groups_get(
    int unit);


uint32 dnx_data_nif_flexe_max_group_id_get(
    int unit);


uint32 dnx_data_nif_flexe_min_group_id_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_tx_delay_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_pcs_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_phy_speeds_get(
    int unit);


uint32 dnx_data_nif_flexe_phy_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_flexe_max_flexe_core_speed_get(
    int unit);


uint32 dnx_data_nif_flexe_flexe_core_clock_source_pm_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nof_slots_get(
    int unit);


uint32 dnx_data_nif_flexe_min_client_id_get(
    int unit);


uint32 dnx_data_nif_flexe_max_client_id_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_instances_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_lphys_get(
    int unit);


uint32 dnx_data_nif_flexe_distributed_ilu_id_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nif_rate_centralized_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nif_rate_distributed_get(
    int unit);


uint32 dnx_data_nif_flexe_max_ilu_rate_distributed_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_core_ports_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_cal_nof_slots_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_cal_granularity_get(
    int unit);


uint32 dnx_data_nif_flexe_non_5g_slot_bw_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_cal_slot_nof_bits_get(
    int unit);


uint32 dnx_data_nif_flexe_flexe_mode_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get(
    int unit);


uint32 dnx_data_nif_flexe_device_slot_mode_get(
    int unit);



typedef enum
{
    dnx_data_nif_flexe_table_phy_info,
    dnx_data_nif_flexe_table_supported_pms,
    dnx_data_nif_flexe_table_client_speed_granularity,
    dnx_data_nif_flexe_table_nb_tx_delay_table,

    
    _dnx_data_nif_flexe_table_nof
} dnx_data_nif_flexe_table_e;



const dnx_data_nif_flexe_phy_info_t * dnx_data_nif_flexe_phy_info_get(
    int unit,
    int idx);


const dnx_data_nif_flexe_supported_pms_t * dnx_data_nif_flexe_supported_pms_get(
    int unit,
    int framer_idx);


const dnx_data_nif_flexe_client_speed_granularity_t * dnx_data_nif_flexe_client_speed_granularity_get(
    int unit,
    int slot_mode);


const dnx_data_nif_flexe_nb_tx_delay_table_t * dnx_data_nif_flexe_nb_tx_delay_table_get(
    int unit,
    int pm_index);



shr_error_e dnx_data_nif_flexe_phy_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_flexe_supported_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_flexe_client_speed_granularity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_flexe_nb_tx_delay_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_flexe_phy_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_supported_pms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_client_speed_granularity_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_nb_tx_delay_table_info_get(
    int unit);






int dnx_data_nif_wbc_feature_get(
    int unit,
    dnx_data_nif_wbc_feature_e feature);



typedef enum
{

    
    _dnx_data_nif_wbc_define_nof
} dnx_data_nif_wbc_define_e;




typedef enum
{

    
    _dnx_data_nif_wbc_table_nof
} dnx_data_nif_wbc_table_e;









int dnx_data_nif_prd_feature_get(
    int unit,
    dnx_data_nif_prd_feature_e feature);



typedef enum
{
    dnx_data_nif_prd_define_nof_control_frames,
    dnx_data_nif_prd_define_nof_ether_type_codes,
    dnx_data_nif_prd_define_nof_tcam_entries,
    dnx_data_nif_prd_define_tcam_key_offset_size,
    dnx_data_nif_prd_define_nof_mpls_special_labels,
    dnx_data_nif_prd_define_nof_priorities,
    dnx_data_nif_prd_define_custom_ether_type_code_min,
    dnx_data_nif_prd_define_custom_ether_type_code_max,
    dnx_data_nif_prd_define_ether_type_code_max,
    dnx_data_nif_prd_define_ether_type_max,
    dnx_data_nif_prd_define_rmc_threshold_max,
    dnx_data_nif_prd_define_cdu_rmc_threshold_max,
    dnx_data_nif_prd_define_hrf_threshold_max,
    dnx_data_nif_prd_define_mpls_special_label_max,
    dnx_data_nif_prd_define_flex_key_offset_key_max,
    dnx_data_nif_prd_define_flex_key_offset_result_max,
    dnx_data_nif_prd_define_tpid_max,
    dnx_data_nif_prd_define_nof_clu_port_profiles,
    dnx_data_nif_prd_define_nof_framer_port_profiles,
    dnx_data_nif_prd_define_nof_ofr_port_profiles,
    dnx_data_nif_prd_define_rmc_fifo_2_threshold_resolution,
    dnx_data_nif_prd_define_nof_ofr_nif_interfaces,
    dnx_data_nif_prd_define_nof_prd_counters,

    
    _dnx_data_nif_prd_define_nof
} dnx_data_nif_prd_define_e;



uint32 dnx_data_nif_prd_nof_control_frames_get(
    int unit);


uint32 dnx_data_nif_prd_nof_ether_type_codes_get(
    int unit);


uint32 dnx_data_nif_prd_nof_tcam_entries_get(
    int unit);


uint32 dnx_data_nif_prd_tcam_key_offset_size_get(
    int unit);


uint32 dnx_data_nif_prd_nof_mpls_special_labels_get(
    int unit);


uint32 dnx_data_nif_prd_nof_priorities_get(
    int unit);


uint32 dnx_data_nif_prd_custom_ether_type_code_min_get(
    int unit);


uint32 dnx_data_nif_prd_custom_ether_type_code_max_get(
    int unit);


uint32 dnx_data_nif_prd_ether_type_code_max_get(
    int unit);


uint32 dnx_data_nif_prd_ether_type_max_get(
    int unit);


uint32 dnx_data_nif_prd_rmc_threshold_max_get(
    int unit);


uint32 dnx_data_nif_prd_cdu_rmc_threshold_max_get(
    int unit);


uint32 dnx_data_nif_prd_hrf_threshold_max_get(
    int unit);


uint32 dnx_data_nif_prd_mpls_special_label_max_get(
    int unit);


uint32 dnx_data_nif_prd_flex_key_offset_key_max_get(
    int unit);


uint32 dnx_data_nif_prd_flex_key_offset_result_max_get(
    int unit);


uint32 dnx_data_nif_prd_tpid_max_get(
    int unit);


uint32 dnx_data_nif_prd_nof_clu_port_profiles_get(
    int unit);


uint32 dnx_data_nif_prd_nof_framer_port_profiles_get(
    int unit);


uint32 dnx_data_nif_prd_nof_ofr_port_profiles_get(
    int unit);


uint32 dnx_data_nif_prd_rmc_fifo_2_threshold_resolution_get(
    int unit);


uint32 dnx_data_nif_prd_nof_ofr_nif_interfaces_get(
    int unit);


uint32 dnx_data_nif_prd_nof_prd_counters_get(
    int unit);



typedef enum
{
    dnx_data_nif_prd_table_ether_type,
    dnx_data_nif_prd_table_port_profile_map,

    
    _dnx_data_nif_prd_table_nof
} dnx_data_nif_prd_table_e;



const dnx_data_nif_prd_ether_type_t * dnx_data_nif_prd_ether_type_get(
    int unit,
    int ether_type_code);


const dnx_data_nif_prd_port_profile_map_t * dnx_data_nif_prd_port_profile_map_get(
    int unit,
    int type);



shr_error_e dnx_data_nif_prd_ether_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_prd_port_profile_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_prd_ether_type_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_prd_port_profile_map_info_get(
    int unit);






int dnx_data_nif_portmod_feature_get(
    int unit,
    dnx_data_nif_portmod_feature_e feature);



typedef enum
{
    dnx_data_nif_portmod_define_pm_types_nof,

    
    _dnx_data_nif_portmod_define_nof
} dnx_data_nif_portmod_define_e;



uint32 dnx_data_nif_portmod_pm_types_nof_get(
    int unit);



typedef enum
{
    dnx_data_nif_portmod_table_pm_types_and_interfaces,

    
    _dnx_data_nif_portmod_table_nof
} dnx_data_nif_portmod_table_e;



const dnx_data_nif_portmod_pm_types_and_interfaces_t * dnx_data_nif_portmod_pm_types_and_interfaces_get(
    int unit,
    int index);



shr_error_e dnx_data_nif_portmod_pm_types_and_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_portmod_pm_types_and_interfaces_info_get(
    int unit);






int dnx_data_nif_scheduler_feature_get(
    int unit,
    dnx_data_nif_scheduler_feature_e feature);



typedef enum
{
    dnx_data_nif_scheduler_define_rate_per_ethu_bit,
    dnx_data_nif_scheduler_define_rate_per_ilu_bit,
    dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit,
    dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit,
    dnx_data_nif_scheduler_define_nof_weight_bits,
    dnx_data_nif_scheduler_define_nof_rmc_bits,
    dnx_data_nif_scheduler_define_dynamic_scheduling_rate_resolution,
    dnx_data_nif_scheduler_define_rmc_map_table_nof_entries,
    dnx_data_nif_scheduler_define_rmc_map_table_entry_resolution,

    
    _dnx_data_nif_scheduler_define_nof
} dnx_data_nif_scheduler_define_e;



uint32 dnx_data_nif_scheduler_rate_per_ethu_bit_get(
    int unit);


uint32 dnx_data_nif_scheduler_rate_per_ilu_bit_get(
    int unit);


uint32 dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get(
    int unit);


uint32 dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get(
    int unit);


uint32 dnx_data_nif_scheduler_nof_weight_bits_get(
    int unit);


uint32 dnx_data_nif_scheduler_nof_rmc_bits_get(
    int unit);


uint32 dnx_data_nif_scheduler_dynamic_scheduling_rate_resolution_get(
    int unit);


uint32 dnx_data_nif_scheduler_rmc_map_table_nof_entries_get(
    int unit);


uint32 dnx_data_nif_scheduler_rmc_map_table_entry_resolution_get(
    int unit);



typedef enum
{
    dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map,

    
    _dnx_data_nif_scheduler_table_nof
} dnx_data_nif_scheduler_table_e;



const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t * dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_get(
    int unit,
    int rmc_id);



shr_error_e dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_info_get(
    int unit);






int dnx_data_nif_dbal_feature_get(
    int unit,
    dnx_data_nif_dbal_feature_e feature);



typedef enum
{
    dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size,
    dnx_data_nif_dbal_define_clu_rx_rmc_entry_size,
    dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_nbu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_mgu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_nbu_tx_threshold_after_ovf_entry_size,
    dnx_data_nif_dbal_define_mgu_tx_threshold_after_ovf_entry_size,
    dnx_data_nif_dbal_define_nbu_rx_fifo_srstn_entry_size,
    dnx_data_nif_dbal_define_mgu_rx_fifo_srstn_entry_size,
    dnx_data_nif_dbal_define_ilu_burst_min_entry_size,
    dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size,
    dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_nbu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_mgu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size,
    dnx_data_nif_dbal_define_tx_credits_bits,
    dnx_data_nif_dbal_define_nof_rx_fifo_counters,
    dnx_data_nif_dbal_define_nof_tx_fifo_counters,

    
    _dnx_data_nif_dbal_define_nof
} dnx_data_nif_dbal_define_e;



uint32 dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_clu_rx_rmc_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_nbu_tx_start_threshold_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_mgu_tx_start_threshold_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_nbu_tx_threshold_after_ovf_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_mgu_tx_threshold_after_ovf_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_nbu_rx_fifo_srstn_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_mgu_rx_fifo_srstn_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_ilu_burst_min_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_nbu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_mgu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get(
    int unit);


uint32 dnx_data_nif_dbal_tx_credits_bits_get(
    int unit);


uint32 dnx_data_nif_dbal_nof_rx_fifo_counters_get(
    int unit);


uint32 dnx_data_nif_dbal_nof_tx_fifo_counters_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_dbal_table_nof
} dnx_data_nif_dbal_table_e;









int dnx_data_nif_features_feature_get(
    int unit,
    dnx_data_nif_features_feature_e feature);



typedef enum
{

    
    _dnx_data_nif_features_define_nof
} dnx_data_nif_features_define_e;




typedef enum
{

    
    _dnx_data_nif_features_table_nof
} dnx_data_nif_features_table_e;









int dnx_data_nif_arb_feature_get(
    int unit,
    dnx_data_nif_arb_feature_e feature);



typedef enum
{
    dnx_data_nif_arb_define_nof_contexts,
    dnx_data_nif_arb_define_nof_rx_qpms,
    dnx_data_nif_arb_define_nof_rx_ppms_in_qpm,
    dnx_data_nif_arb_define_nof_lanes_in_qpm,
    dnx_data_nif_arb_define_nof_map_destinations,
    dnx_data_nif_arb_define_max_calendar_length,
    dnx_data_nif_arb_define_nof_rx_sources,
    dnx_data_nif_arb_define_rx_sch_calendar_nof_modes,
    dnx_data_nif_arb_define_rx_qpm_calendar_length,
    dnx_data_nif_arb_define_rx_sch_calendar_length,
    dnx_data_nif_arb_define_cdpm_calendar_length,
    dnx_data_nif_arb_define_tmac_calendar_length,
    dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients,
    dnx_data_nif_arb_define_rx_sch_calendar_nof_clients,
    dnx_data_nif_arb_define_rx_qpm_port_speed_granularity,
    dnx_data_nif_arb_define_rx_sch_port_speed_granularity,
    dnx_data_nif_arb_define_tx_eth_port_speed_granularity,
    dnx_data_nif_arb_define_tx_tmac_port_speed_granularity,
    dnx_data_nif_arb_define_tx_tmac_nof_sections,
    dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity,
    dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar,
    dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar,
    dnx_data_nif_arb_define_nof_entries_in_tmac_calendar,
    dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar,
    dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar,
    dnx_data_nif_arb_define_min_port_speed_for_link_list_section,
    dnx_data_nif_arb_define_link_list_sections_denominator,
    dnx_data_nif_arb_define_high_start_tx_speed_threshold,
    dnx_data_nif_arb_define_low_start_tx_speed_threshold,
    dnx_data_nif_arb_define_high_start_tx_speed_value,
    dnx_data_nif_arb_define_middle_start_tx_speed_value,
    dnx_data_nif_arb_define_low_start_tx_speed_value,
    dnx_data_nif_arb_define_max_value_for_tx_start_threshold,
    dnx_data_nif_arb_define_rx_pm_default_min_pkt_size,

    
    _dnx_data_nif_arb_define_nof
} dnx_data_nif_arb_define_e;



uint32 dnx_data_nif_arb_nof_contexts_get(
    int unit);


uint32 dnx_data_nif_arb_nof_rx_qpms_get(
    int unit);


uint32 dnx_data_nif_arb_nof_rx_ppms_in_qpm_get(
    int unit);


uint32 dnx_data_nif_arb_nof_lanes_in_qpm_get(
    int unit);


uint32 dnx_data_nif_arb_nof_map_destinations_get(
    int unit);


uint32 dnx_data_nif_arb_max_calendar_length_get(
    int unit);


uint32 dnx_data_nif_arb_nof_rx_sources_get(
    int unit);


uint32 dnx_data_nif_arb_rx_sch_calendar_nof_modes_get(
    int unit);


uint32 dnx_data_nif_arb_rx_qpm_calendar_length_get(
    int unit);


uint32 dnx_data_nif_arb_rx_sch_calendar_length_get(
    int unit);


uint32 dnx_data_nif_arb_cdpm_calendar_length_get(
    int unit);


uint32 dnx_data_nif_arb_tmac_calendar_length_get(
    int unit);


uint32 dnx_data_nif_arb_rx_qpm_calendar_nof_clients_get(
    int unit);


uint32 dnx_data_nif_arb_rx_sch_calendar_nof_clients_get(
    int unit);


uint32 dnx_data_nif_arb_rx_qpm_port_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_arb_rx_sch_port_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_arb_tx_eth_port_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_arb_tx_tmac_port_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_arb_tx_tmac_nof_sections_get(
    int unit);


uint32 dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_get(
    int unit);


uint32 dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_get(
    int unit);


uint32 dnx_data_nif_arb_nof_entries_in_tmac_calendar_get(
    int unit);


uint32 dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_get(
    int unit);


uint32 dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_get(
    int unit);


uint32 dnx_data_nif_arb_min_port_speed_for_link_list_section_get(
    int unit);


uint32 dnx_data_nif_arb_link_list_sections_denominator_get(
    int unit);


uint32 dnx_data_nif_arb_high_start_tx_speed_threshold_get(
    int unit);


uint32 dnx_data_nif_arb_low_start_tx_speed_threshold_get(
    int unit);


uint32 dnx_data_nif_arb_high_start_tx_speed_value_get(
    int unit);


uint32 dnx_data_nif_arb_middle_start_tx_speed_value_get(
    int unit);


uint32 dnx_data_nif_arb_low_start_tx_speed_value_get(
    int unit);


uint32 dnx_data_nif_arb_max_value_for_tx_start_threshold_get(
    int unit);


uint32 dnx_data_nif_arb_rx_pm_default_min_pkt_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_arb_table_nof
} dnx_data_nif_arb_table_e;









int dnx_data_nif_ofr_feature_get(
    int unit,
    dnx_data_nif_ofr_feature_e feature);



typedef enum
{
    dnx_data_nif_ofr_define_nof_contexts,
    dnx_data_nif_ofr_define_nof_rmc_per_priority_group,
    dnx_data_nif_ofr_define_nof_total_rmcs,
    dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group,
    dnx_data_nif_ofr_define_nof_rx_memory_groups,
    dnx_data_nif_ofr_define_nof_rx_memory_sections,
    dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section,
    dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity,
    dnx_data_nif_ofr_define_rx_sch_granularity,
    dnx_data_nif_ofr_define_nof_bytes_per_memory_section,

    
    _dnx_data_nif_ofr_define_nof
} dnx_data_nif_ofr_define_e;



uint32 dnx_data_nif_ofr_nof_contexts_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_rmc_per_priority_group_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_total_rmcs_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_rx_mem_sections_per_group_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_rx_memory_groups_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_rx_memory_sections_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_rx_memory_entries_per_section_get(
    int unit);


uint32 dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_ofr_rx_sch_granularity_get(
    int unit);


uint32 dnx_data_nif_ofr_nof_bytes_per_memory_section_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_ofr_table_nof
} dnx_data_nif_ofr_table_e;









int dnx_data_nif_oft_feature_get(
    int unit,
    dnx_data_nif_oft_feature_e feature);



typedef enum
{
    dnx_data_nif_oft_define_calendar_length,
    dnx_data_nif_oft_define_nof_sections,
    dnx_data_nif_oft_define_nof_internal_calendar_entries,
    dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar,
    dnx_data_nif_oft_define_nof_contexts,
    dnx_data_nif_oft_define_port_speed_granularity,
    dnx_data_nif_oft_define_calendar_speed_granularity,

    
    _dnx_data_nif_oft_define_nof
} dnx_data_nif_oft_define_e;



uint32 dnx_data_nif_oft_calendar_length_get(
    int unit);


uint32 dnx_data_nif_oft_nof_sections_get(
    int unit);


uint32 dnx_data_nif_oft_nof_internal_calendar_entries_get(
    int unit);


uint32 dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_get(
    int unit);


uint32 dnx_data_nif_oft_nof_contexts_get(
    int unit);


uint32 dnx_data_nif_oft_port_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_oft_calendar_speed_granularity_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_oft_table_nof
} dnx_data_nif_oft_table_e;









int dnx_data_nif_signal_quality_feature_get(
    int unit,
    dnx_data_nif_signal_quality_feature_e feature);



typedef enum
{
    dnx_data_nif_signal_quality_define_is_debug_enabled,
    dnx_data_nif_signal_quality_define_max_degrade_threshold,
    dnx_data_nif_signal_quality_define_min_degrade_threshold,
    dnx_data_nif_signal_quality_define_max_fail_threshold,
    dnx_data_nif_signal_quality_define_min_fail_threshold,
    dnx_data_nif_signal_quality_define_max_interval,
    dnx_data_nif_signal_quality_define_min_interval,

    
    _dnx_data_nif_signal_quality_define_nof
} dnx_data_nif_signal_quality_define_e;



uint32 dnx_data_nif_signal_quality_is_debug_enabled_get(
    int unit);


uint32 dnx_data_nif_signal_quality_max_degrade_threshold_get(
    int unit);


uint32 dnx_data_nif_signal_quality_min_degrade_threshold_get(
    int unit);


uint32 dnx_data_nif_signal_quality_max_fail_threshold_get(
    int unit);


uint32 dnx_data_nif_signal_quality_min_fail_threshold_get(
    int unit);


uint32 dnx_data_nif_signal_quality_max_interval_get(
    int unit);


uint32 dnx_data_nif_signal_quality_min_interval_get(
    int unit);



typedef enum
{
    dnx_data_nif_signal_quality_table_supported_fecs,
    dnx_data_nif_signal_quality_table_hw_based_solution,

    
    _dnx_data_nif_signal_quality_table_nof
} dnx_data_nif_signal_quality_table_e;



const dnx_data_nif_signal_quality_supported_fecs_t * dnx_data_nif_signal_quality_supported_fecs_get(
    int unit,
    int fec_type);


const dnx_data_nif_signal_quality_hw_based_solution_t * dnx_data_nif_signal_quality_hw_based_solution_get(
    int unit,
    int pm_type);



shr_error_e dnx_data_nif_signal_quality_supported_fecs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_signal_quality_hw_based_solution_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_signal_quality_supported_fecs_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_signal_quality_hw_based_solution_info_get(
    int unit);






int dnx_data_nif_otn_feature_get(
    int unit,
    dnx_data_nif_otn_feature_e feature);



typedef enum
{
    dnx_data_nif_otn_define_nof_units,
    dnx_data_nif_otn_define_nof_lanes,
    dnx_data_nif_otn_define_nof_groups,
    dnx_data_nif_otn_define_nof_lanes_per_group,

    
    _dnx_data_nif_otn_define_nof
} dnx_data_nif_otn_define_e;



uint32 dnx_data_nif_otn_nof_units_get(
    int unit);


uint32 dnx_data_nif_otn_nof_lanes_get(
    int unit);


uint32 dnx_data_nif_otn_nof_groups_get(
    int unit);


uint32 dnx_data_nif_otn_nof_lanes_per_group_get(
    int unit);



typedef enum
{
    dnx_data_nif_otn_table_supported_interfaces,

    
    _dnx_data_nif_otn_table_nof
} dnx_data_nif_otn_table_e;



const dnx_data_nif_otn_supported_interfaces_t * dnx_data_nif_otn_supported_interfaces_get(
    int unit,
    int index);



shr_error_e dnx_data_nif_otn_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_otn_supported_interfaces_info_get(
    int unit);






int dnx_data_nif_flr_feature_get(
    int unit,
    dnx_data_nif_flr_feature_e feature);



typedef enum
{
    dnx_data_nif_flr_define_max_tick_unit,
    dnx_data_nif_flr_define_max_tick_count,
    dnx_data_nif_flr_define_timer_granularity,

    
    _dnx_data_nif_flr_define_nof
} dnx_data_nif_flr_define_e;



uint32 dnx_data_nif_flr_max_tick_unit_get(
    int unit);


uint32 dnx_data_nif_flr_max_tick_count_get(
    int unit);


uint32 dnx_data_nif_flr_timer_granularity_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_flr_table_nof
} dnx_data_nif_flr_table_e;






shr_error_e dnx_data_nif_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

