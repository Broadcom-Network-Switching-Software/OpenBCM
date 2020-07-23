

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_NIF_H_

#define _DNX_DATA_INTERNAL_NIF_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_nif_submodule_global,
    dnx_data_nif_submodule_phys,
    dnx_data_nif_submodule_ilkn,
    dnx_data_nif_submodule_eth,
    dnx_data_nif_submodule_simulator,
    dnx_data_nif_submodule_flexe,
    dnx_data_nif_submodule_prd,
    dnx_data_nif_submodule_portmod,
    dnx_data_nif_submodule_scheduler,
    dnx_data_nif_submodule_dbal,
    dnx_data_nif_submodule_features,

    
    _dnx_data_nif_submodule_nof
} dnx_data_nif_submodule_e;








int dnx_data_nif_global_feature_get(
    int unit,
    dnx_data_nif_global_feature_e feature);



typedef enum
{
    dnx_data_nif_global_define_nof_lcplls,
    dnx_data_nif_global_define_max_core_access_per_port,
    dnx_data_nif_global_define_nof_nif_interfaces_per_core,
    dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity,
    dnx_data_nif_global_define_reassembler_fifo_threshold,
    dnx_data_nif_global_define_last_port_led_scan,
    dnx_data_nif_global_define_start_tx_threshold_global,

    
    _dnx_data_nif_global_define_nof
} dnx_data_nif_global_define_e;



uint32 dnx_data_nif_global_nof_lcplls_get(
    int unit);


uint32 dnx_data_nif_global_max_core_access_per_port_get(
    int unit);


uint32 dnx_data_nif_global_nof_nif_interfaces_per_core_get(
    int unit);


uint32 dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get(
    int unit);


uint32 dnx_data_nif_global_reassembler_fifo_threshold_get(
    int unit);


uint32 dnx_data_nif_global_last_port_led_scan_get(
    int unit);


uint32 dnx_data_nif_global_start_tx_threshold_global_get(
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
    dnx_data_nif_phys_define_nof_phys_per_core,
    dnx_data_nif_phys_define_pm8x50_gen,
    dnx_data_nif_phys_define_is_pam4_speed_supported,

    
    _dnx_data_nif_phys_define_nof
} dnx_data_nif_phys_define_e;



uint32 dnx_data_nif_phys_nof_phys_get(
    int unit);


uint32 dnx_data_nif_phys_nof_phys_per_core_get(
    int unit);


uint32 dnx_data_nif_phys_pm8x50_gen_get(
    int unit);


uint32 dnx_data_nif_phys_is_pam4_speed_supported_get(
    int unit);



typedef enum
{
    dnx_data_nif_phys_table_general,
    dnx_data_nif_phys_table_polarity,
    dnx_data_nif_phys_table_core_phys_map,
    dnx_data_nif_phys_table_vco_div,

    
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



const dnxc_data_table_info_t * dnx_data_nif_phys_general_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_polarity_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_core_phys_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_phys_vco_div_info_get(
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
    dnx_data_nif_ilkn_define_is_20G_speed_supported,
    dnx_data_nif_ilkn_define_is_25G_speed_supported,
    dnx_data_nif_ilkn_define_is_27G_speed_supported,
    dnx_data_nif_ilkn_define_is_28G_speed_supported,
    dnx_data_nif_ilkn_define_is_53G_speed_supported,
    dnx_data_nif_ilkn_define_is_nif_56G_speed_supported,
    dnx_data_nif_ilkn_define_is_fabric_56G_speed_supported,
    dnx_data_nif_ilkn_define_pad_size,
    dnx_data_nif_ilkn_define_max_nof_ifs,
    dnx_data_nif_ilkn_define_max_nof_elk_ifs,
    dnx_data_nif_ilkn_define_port_0_status_intr_id,
    dnx_data_nif_ilkn_define_port_1_status_intr_id,

    
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


uint32 dnx_data_nif_ilkn_is_20G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_25G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_27G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_28G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_53G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_nif_56G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_is_fabric_56G_speed_supported_get(
    int unit);


uint32 dnx_data_nif_ilkn_pad_size_get(
    int unit);


uint32 dnx_data_nif_ilkn_max_nof_ifs_get(
    int unit);


uint32 dnx_data_nif_ilkn_max_nof_elk_ifs_get(
    int unit);


uint32 dnx_data_nif_ilkn_port_0_status_intr_id_get(
    int unit);


uint32 dnx_data_nif_ilkn_port_1_status_intr_id_get(
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
    dnx_data_nif_ilkn_table_ilkn_pms,
    dnx_data_nif_ilkn_table_nif_lanes_map,
    dnx_data_nif_ilkn_table_fabric_lanes_map,
    dnx_data_nif_ilkn_table_start_tx_threshold_table,

    
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


const dnx_data_nif_ilkn_ilkn_pms_t * dnx_data_nif_ilkn_ilkn_pms_get(
    int unit,
    int ilkn_id);


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


shr_error_e dnx_data_nif_ilkn_ilkn_pms_entry_str_get(
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


const dnxc_data_table_info_t * dnx_data_nif_ilkn_ilkn_pms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_nif_lanes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_fabric_lanes_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_ilkn_start_tx_threshold_table_info_get(
    int unit);






int dnx_data_nif_eth_feature_get(
    int unit,
    dnx_data_nif_eth_feature_e feature);



typedef enum
{
    dnx_data_nif_eth_define_cdu_nof,
    dnx_data_nif_eth_define_clu_nof,
    dnx_data_nif_eth_define_cdum_nof,
    dnx_data_nif_eth_define_nof_pms_in_cdu,
    dnx_data_nif_eth_define_nof_pms_in_clu,
    dnx_data_nif_eth_define_total_nof_ethu_pms_in_device,
    dnx_data_nif_eth_define_nof_cdu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_clu_lanes_in_pm,
    dnx_data_nif_eth_define_nof_cdu_pms,
    dnx_data_nif_eth_define_nof_clu_pms,
    dnx_data_nif_eth_define_ethu_nof,
    dnx_data_nif_eth_define_ethu_nof_per_core,
    dnx_data_nif_eth_define_cdu_nof_per_core,
    dnx_data_nif_eth_define_clu_nof_per_core,
    dnx_data_nif_eth_define_nof_lanes_in_cdu,
    dnx_data_nif_eth_define_nof_lanes_in_clu,
    dnx_data_nif_eth_define_max_nof_lanes_in_ethu,
    dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device,
    dnx_data_nif_eth_define_cdu_mac_mode_config_nof,
    dnx_data_nif_eth_define_mac_mode_config_lanes_nof,
    dnx_data_nif_eth_define_cdu_mac_nof,
    dnx_data_nif_eth_define_mac_lanes_nof,
    dnx_data_nif_eth_define_ethu_logical_fifo_nof,
    dnx_data_nif_eth_define_cdu_memory_entries_nof,
    dnx_data_nif_eth_define_clu_memory_entries_nof,
    dnx_data_nif_eth_define_priority_group_nof_entries_min,
    dnx_data_nif_eth_define_priority_groups_nof,
    dnx_data_nif_eth_define_pad_size_min,
    dnx_data_nif_eth_define_pad_size_max,
    dnx_data_nif_eth_define_packet_size_max,
    dnx_data_nif_eth_define_an_max_nof_abilities,
    dnx_data_nif_eth_define_phy_map_granularity,
    dnx_data_nif_eth_define_is_400G_supported,

    
    _dnx_data_nif_eth_define_nof
} dnx_data_nif_eth_define_e;



uint32 dnx_data_nif_eth_cdu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_clu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdum_nof_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_cdu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_pms_in_clu_get(
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


uint32 dnx_data_nif_eth_ethu_nof_get(
    int unit);


uint32 dnx_data_nif_eth_ethu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_clu_nof_per_core_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_cdu_get(
    int unit);


uint32 dnx_data_nif_eth_nof_lanes_in_clu_get(
    int unit);


uint32 dnx_data_nif_eth_max_nof_lanes_in_ethu_get(
    int unit);


uint32 dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_mac_mode_config_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mac_mode_config_lanes_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_mac_nof_get(
    int unit);


uint32 dnx_data_nif_eth_mac_lanes_nof_get(
    int unit);


uint32 dnx_data_nif_eth_ethu_logical_fifo_nof_get(
    int unit);


uint32 dnx_data_nif_eth_cdu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_clu_memory_entries_nof_get(
    int unit);


uint32 dnx_data_nif_eth_priority_group_nof_entries_min_get(
    int unit);


uint32 dnx_data_nif_eth_priority_groups_nof_get(
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


uint32 dnx_data_nif_eth_is_400G_supported_get(
    int unit);



typedef enum
{
    dnx_data_nif_eth_table_start_tx_threshold_table,
    dnx_data_nif_eth_table_pm_properties,
    dnx_data_nif_eth_table_ethu_properties,
    dnx_data_nif_eth_table_phy_map,
    dnx_data_nif_eth_table_max_speed,
    dnx_data_nif_eth_table_supported_interfaces,
    dnx_data_nif_eth_table_supported_an_abilities,

    
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


const dnx_data_nif_eth_max_speed_t * dnx_data_nif_eth_max_speed_get(
    int unit,
    int lane_num);


const dnx_data_nif_eth_supported_interfaces_t * dnx_data_nif_eth_supported_interfaces_get(
    int unit,
    int idx);


const dnx_data_nif_eth_supported_an_abilities_t * dnx_data_nif_eth_supported_an_abilities_get(
    int unit,
    int idx);



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


shr_error_e dnx_data_nif_eth_supported_an_abilities_entry_str_get(
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


const dnxc_data_table_info_t * dnx_data_nif_eth_max_speed_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_supported_interfaces_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_eth_supported_an_abilities_info_get(
    int unit);






int dnx_data_nif_simulator_feature_get(
    int unit,
    dnx_data_nif_simulator_feature_e feature);



typedef enum
{
    dnx_data_nif_simulator_define_cdu_type,
    dnx_data_nif_simulator_define_clu_type,

    
    _dnx_data_nif_simulator_define_nof
} dnx_data_nif_simulator_define_e;



uint32 dnx_data_nif_simulator_cdu_type_get(
    int unit);


uint32 dnx_data_nif_simulator_clu_type_get(
    int unit);



typedef enum
{

    
    _dnx_data_nif_simulator_table_nof
} dnx_data_nif_simulator_table_e;









int dnx_data_nif_flexe_feature_get(
    int unit,
    dnx_data_nif_flexe_feature_e feature);



typedef enum
{
    dnx_data_nif_flexe_define_feu_nof,
    dnx_data_nif_flexe_define_nof_clients,
    dnx_data_nif_flexe_define_nof_flexe_clients,
    dnx_data_nif_flexe_define_nof_special_clients,
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
    dnx_data_nif_flexe_define_nof_logical_fifos,
    dnx_data_nif_flexe_define_sar_channel_base,
    dnx_data_nif_flexe_define_mac_channel_base,
    dnx_data_nif_flexe_define_client_speed_granularity,
    dnx_data_nif_flexe_define_nof_sb_rx_fifos,
    dnx_data_nif_flexe_define_oam_client_channel,
    dnx_data_nif_flexe_define_ptp_client_channel,
    dnx_data_nif_flexe_define_oam_tx_cal_slot,
    dnx_data_nif_flexe_define_ptp_tx_cal_slot,
    dnx_data_nif_flexe_define_priority_groups_nof,
    dnx_data_nif_flexe_define_max_nof_slots,
    dnx_data_nif_flexe_define_min_client_id,
    dnx_data_nif_flexe_define_max_client_id,
    dnx_data_nif_flexe_define_ptp_channel_max_bandwidth,
    dnx_data_nif_flexe_define_oam_channel_max_bandwidth,
    dnx_data_nif_flexe_define_nof_mem_entries_per_slot,
    dnx_data_nif_flexe_define_ptp_oam_fifo_entries_in_sb_rx,
    dnx_data_nif_flexe_define_nof_flexe_instances,
    dnx_data_nif_flexe_define_nof_flexe_lphys,
    dnx_data_nif_flexe_define_nof_sar_timeslots,
    dnx_data_nif_flexe_define_nof_mac_timeslots,
    dnx_data_nif_flexe_define_distributed_ilu_id,
    dnx_data_nif_flexe_define_max_nif_rate_centralized,
    dnx_data_nif_flexe_define_rmc_cal_nof_slots,
    dnx_data_nif_flexe_define_max_nif_rate_distributed,
    dnx_data_nif_flexe_define_max_ilu_rate_distributed,
    dnx_data_nif_flexe_define_ilkn_burst_size_in_feu,
    dnx_data_nif_flexe_define_average_ipg_for_l1,
    dnx_data_nif_flexe_define_mac_tx_threshold,
    dnx_data_nif_flexe_define_flexe_mode,
    dnx_data_nif_flexe_define_nof_pms_per_feu,
    dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode,

    
    _dnx_data_nif_flexe_define_nof
} dnx_data_nif_flexe_define_e;



uint32 dnx_data_nif_flexe_feu_nof_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_clients_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_clients_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_special_clients_get(
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


uint32 dnx_data_nif_flexe_nof_logical_fifos_get(
    int unit);


uint32 dnx_data_nif_flexe_sar_channel_base_get(
    int unit);


uint32 dnx_data_nif_flexe_mac_channel_base_get(
    int unit);


uint32 dnx_data_nif_flexe_client_speed_granularity_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_sb_rx_fifos_get(
    int unit);


uint32 dnx_data_nif_flexe_oam_client_channel_get(
    int unit);


uint32 dnx_data_nif_flexe_ptp_client_channel_get(
    int unit);


uint32 dnx_data_nif_flexe_oam_tx_cal_slot_get(
    int unit);


uint32 dnx_data_nif_flexe_ptp_tx_cal_slot_get(
    int unit);


uint32 dnx_data_nif_flexe_priority_groups_nof_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nof_slots_get(
    int unit);


uint32 dnx_data_nif_flexe_min_client_id_get(
    int unit);


uint32 dnx_data_nif_flexe_max_client_id_get(
    int unit);


uint32 dnx_data_nif_flexe_ptp_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_flexe_oam_channel_max_bandwidth_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_mem_entries_per_slot_get(
    int unit);


uint32 dnx_data_nif_flexe_ptp_oam_fifo_entries_in_sb_rx_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_instances_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_flexe_lphys_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_sar_timeslots_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_mac_timeslots_get(
    int unit);


uint32 dnx_data_nif_flexe_distributed_ilu_id_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nif_rate_centralized_get(
    int unit);


uint32 dnx_data_nif_flexe_rmc_cal_nof_slots_get(
    int unit);


uint32 dnx_data_nif_flexe_max_nif_rate_distributed_get(
    int unit);


uint32 dnx_data_nif_flexe_max_ilu_rate_distributed_get(
    int unit);


uint32 dnx_data_nif_flexe_ilkn_burst_size_in_feu_get(
    int unit);


uint32 dnx_data_nif_flexe_average_ipg_for_l1_get(
    int unit);


uint32 dnx_data_nif_flexe_mac_tx_threshold_get(
    int unit);


uint32 dnx_data_nif_flexe_flexe_mode_get(
    int unit);


uint32 dnx_data_nif_flexe_nof_pms_per_feu_get(
    int unit);


uint32 dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get(
    int unit);



typedef enum
{
    dnx_data_nif_flexe_table_phy_info,
    dnx_data_nif_flexe_table_start_tx_threshold_table,
    dnx_data_nif_flexe_table_supported_pms,
    dnx_data_nif_flexe_table_l1_mismatch_rate_table,
    dnx_data_nif_flexe_table_sar_cell_mode_to_size,

    
    _dnx_data_nif_flexe_table_nof
} dnx_data_nif_flexe_table_e;



const dnx_data_nif_flexe_phy_info_t * dnx_data_nif_flexe_phy_info_get(
    int unit,
    int idx);


const dnx_data_nif_flexe_start_tx_threshold_table_t * dnx_data_nif_flexe_start_tx_threshold_table_get(
    int unit,
    int idx);


const dnx_data_nif_flexe_supported_pms_t * dnx_data_nif_flexe_supported_pms_get(
    int unit,
    int feu_idx);


const dnx_data_nif_flexe_l1_mismatch_rate_table_t * dnx_data_nif_flexe_l1_mismatch_rate_table_get(
    int unit,
    int idx);


const dnx_data_nif_flexe_sar_cell_mode_to_size_t * dnx_data_nif_flexe_sar_cell_mode_to_size_get(
    int unit,
    int cell_mode);



shr_error_e dnx_data_nif_flexe_phy_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_flexe_start_tx_threshold_table_entry_str_get(
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


shr_error_e dnx_data_nif_flexe_l1_mismatch_rate_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_nif_flexe_sar_cell_mode_to_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_nif_flexe_phy_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_start_tx_threshold_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_supported_pms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_l1_mismatch_rate_table_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_nif_flexe_sar_cell_mode_to_size_info_get(
    int unit);






int dnx_data_nif_prd_feature_get(
    int unit,
    dnx_data_nif_prd_feature_e feature);



typedef enum
{
    dnx_data_nif_prd_define_nof_control_frames,
    dnx_data_nif_prd_define_nof_ether_type_codes,
    dnx_data_nif_prd_define_nof_tcam_entries,
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
    dnx_data_nif_prd_define_nof_feu_port_profiles,

    
    _dnx_data_nif_prd_define_nof
} dnx_data_nif_prd_define_e;



uint32 dnx_data_nif_prd_nof_control_frames_get(
    int unit);


uint32 dnx_data_nif_prd_nof_ether_type_codes_get(
    int unit);


uint32 dnx_data_nif_prd_nof_tcam_entries_get(
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


uint32 dnx_data_nif_prd_nof_feu_port_profiles_get(
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



typedef enum
{

    
    _dnx_data_nif_scheduler_table_nof
} dnx_data_nif_scheduler_table_e;









int dnx_data_nif_dbal_feature_get(
    int unit,
    dnx_data_nif_dbal_feature_e feature);



typedef enum
{
    dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size,
    dnx_data_nif_dbal_define_clu_rx_rmc_entry_size,
    dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size,
    dnx_data_nif_dbal_define_ilu_burst_min_entry_size,
    dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size,
    dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size,
    dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size,
    dnx_data_nif_dbal_define_tx_credits_bits,

    
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


uint32 dnx_data_nif_dbal_ilu_burst_min_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit);


uint32 dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get(
    int unit);


uint32 dnx_data_nif_dbal_tx_credits_bits_get(
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






shr_error_e dnx_data_nif_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

