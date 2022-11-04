
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_PORT_H_

#define _DNXF_DATA_INTERNAL_PORT_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



typedef enum
{
    dnxf_data_port_submodule_general,
    dnxf_data_port_submodule_stat,
    dnxf_data_port_submodule_static_add,
    dnxf_data_port_submodule_lane_map,
    dnxf_data_port_submodule_pll,
    dnxf_data_port_submodule_synce,
    dnxf_data_port_submodule_retimer,
    dnxf_data_port_submodule_regression,

    
    _dnxf_data_port_submodule_nof
} dnxf_data_port_submodule_e;








int dnxf_data_port_general_feature_get(
    int unit,
    dnxf_data_port_general_feature_e feature);



typedef enum
{
    dnxf_data_port_general_define_nof_links,
    dnxf_data_port_general_define_nof_pms,
    dnxf_data_port_general_define_max_bucket_fill_rate,
    dnxf_data_port_general_define_link_mask,
    dnxf_data_port_general_define_aldwp_max_val,
    dnxf_data_port_general_define_fmac_clock_khz,
    dnxf_data_port_general_define_speed_max,
    dnxf_data_port_general_define_fmac_tx_padding_min_port_speed,
    dnxf_data_port_general_define_nof_ber_proj_error_analyzer_counters,
    dnxf_data_port_general_define_fw_load_method,
    dnxf_data_port_general_define_fw_crc_check,
    dnxf_data_port_general_define_fw_load_verify,
    dnxf_data_port_general_define_mac_bucket_fill_rate,

    
    _dnxf_data_port_general_define_nof
} dnxf_data_port_general_define_e;



uint32 dnxf_data_port_general_nof_links_get(
    int unit);


uint32 dnxf_data_port_general_nof_pms_get(
    int unit);


uint32 dnxf_data_port_general_max_bucket_fill_rate_get(
    int unit);


uint32 dnxf_data_port_general_link_mask_get(
    int unit);


uint32 dnxf_data_port_general_aldwp_max_val_get(
    int unit);


uint32 dnxf_data_port_general_fmac_clock_khz_get(
    int unit);


uint32 dnxf_data_port_general_speed_max_get(
    int unit);


uint32 dnxf_data_port_general_fmac_tx_padding_min_port_speed_get(
    int unit);


uint32 dnxf_data_port_general_nof_ber_proj_error_analyzer_counters_get(
    int unit);


uint32 dnxf_data_port_general_fw_load_method_get(
    int unit);


uint32 dnxf_data_port_general_fw_crc_check_get(
    int unit);


uint32 dnxf_data_port_general_fw_load_verify_get(
    int unit);


uint32 dnxf_data_port_general_mac_bucket_fill_rate_get(
    int unit);



typedef enum
{
    dnxf_data_port_general_table_supported_lanes,
    dnxf_data_port_general_table_fmac_bus_size,
    dnxf_data_port_general_table_supported_interfaces,
    dnxf_data_port_general_table_default_fec,
    dnxf_data_port_general_table_supported_fec_controls,
    dnxf_data_port_general_table_supported_fmac_tx_padding_sizes,
    dnxf_data_port_general_table_fec_compensation_info,
    dnxf_data_port_general_table_phy_controls_features,

    
    _dnxf_data_port_general_table_nof
} dnxf_data_port_general_table_e;



const dnxf_data_port_general_supported_lanes_t * dnxf_data_port_general_supported_lanes_get(
    int unit);


const dnxf_data_port_general_fmac_bus_size_t * dnxf_data_port_general_fmac_bus_size_get(
    int unit,
    int mode);


const dnxf_data_port_general_supported_interfaces_t * dnxf_data_port_general_supported_interfaces_get(
    int unit,
    int index);


const dnxf_data_port_general_default_fec_t * dnxf_data_port_general_default_fec_get(
    int unit,
    int speed);


const dnxf_data_port_general_supported_fec_controls_t * dnxf_data_port_general_supported_fec_controls_get(
    int unit,
    int index);


const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t * dnxf_data_port_general_supported_fmac_tx_padding_sizes_get(
    int unit,
    int index);


const dnxf_data_port_general_fec_compensation_info_t * dnxf_data_port_general_fec_compensation_info_get(
    int unit,
    int fec_type);


const dnxf_data_port_general_phy_controls_features_t * dnxf_data_port_general_phy_controls_features_get(
    int unit,
    int phy_control_type);



shr_error_e dnxf_data_port_general_supported_lanes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_fmac_bus_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_default_fec_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_supported_fec_controls_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_supported_fmac_tx_padding_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_fec_compensation_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_general_phy_controls_features_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_general_supported_lanes_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_fmac_bus_size_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_supported_interfaces_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_default_fec_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_supported_fec_controls_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_supported_fmac_tx_padding_sizes_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_fec_compensation_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_general_phy_controls_features_info_get(
    int unit);






int dnxf_data_port_stat_feature_get(
    int unit,
    dnxf_data_port_stat_feature_e feature);



typedef enum
{
    dnxf_data_port_stat_define_thread_interval,
    dnxf_data_port_stat_define_thread_flags,
    dnxf_data_port_stat_define_thread_counter_pri,
    dnxf_data_port_stat_define_thread_timeout,

    
    _dnxf_data_port_stat_define_nof
} dnxf_data_port_stat_define_e;



uint32 dnxf_data_port_stat_thread_interval_get(
    int unit);


uint32 dnxf_data_port_stat_thread_flags_get(
    int unit);


uint32 dnxf_data_port_stat_thread_counter_pri_get(
    int unit);


uint32 dnxf_data_port_stat_thread_timeout_get(
    int unit);



typedef enum
{

    
    _dnxf_data_port_stat_table_nof
} dnxf_data_port_stat_table_e;









int dnxf_data_port_static_add_feature_get(
    int unit,
    dnxf_data_port_static_add_feature_e feature);



typedef enum
{

    
    _dnxf_data_port_static_add_define_nof
} dnxf_data_port_static_add_define_e;




typedef enum
{
    dnxf_data_port_static_add_table_port_info,
    dnxf_data_port_static_add_table_serdes_lane_config,
    dnxf_data_port_static_add_table_serdes_tx_taps,
    dnxf_data_port_static_add_table_quad_info,
    dnxf_data_port_static_add_table_cable_swap_info,

    
    _dnxf_data_port_static_add_table_nof
} dnxf_data_port_static_add_table_e;



const dnxf_data_port_static_add_port_info_t * dnxf_data_port_static_add_port_info_get(
    int unit,
    int port);


const dnxf_data_port_static_add_serdes_lane_config_t * dnxf_data_port_static_add_serdes_lane_config_get(
    int unit,
    int port);


const dnxf_data_port_static_add_serdes_tx_taps_t * dnxf_data_port_static_add_serdes_tx_taps_get(
    int unit,
    int port);


const dnxf_data_port_static_add_quad_info_t * dnxf_data_port_static_add_quad_info_get(
    int unit,
    int quad);


const dnxf_data_port_static_add_cable_swap_info_t * dnxf_data_port_static_add_cable_swap_info_get(
    int unit,
    int pm);



shr_error_e dnxf_data_port_static_add_port_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_static_add_serdes_lane_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_static_add_serdes_tx_taps_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_static_add_quad_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_static_add_cable_swap_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_static_add_port_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_static_add_serdes_lane_config_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_static_add_serdes_tx_taps_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_static_add_quad_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_static_add_cable_swap_info_info_get(
    int unit);






int dnxf_data_port_lane_map_feature_get(
    int unit,
    dnxf_data_port_lane_map_feature_e feature);



typedef enum
{
    dnxf_data_port_lane_map_define_lane_map_bound,

    
    _dnxf_data_port_lane_map_define_nof
} dnxf_data_port_lane_map_define_e;



uint32 dnxf_data_port_lane_map_lane_map_bound_get(
    int unit);



typedef enum
{
    dnxf_data_port_lane_map_table_info,

    
    _dnxf_data_port_lane_map_table_nof
} dnxf_data_port_lane_map_table_e;



const dnxf_data_port_lane_map_info_t * dnxf_data_port_lane_map_info_get(
    int unit,
    int lane);



shr_error_e dnxf_data_port_lane_map_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_lane_map_info_info_get(
    int unit);






int dnxf_data_port_pll_feature_get(
    int unit,
    dnxf_data_port_pll_feature_e feature);



typedef enum
{
    dnxf_data_port_pll_define_nof_lcpll,
    dnxf_data_port_pll_define_nof_port_in_lcpll,
    dnxf_data_port_pll_define_fabric_clock_freq_in_default,
    dnxf_data_port_pll_define_fabric_clock_freq_out_default,
    dnxf_data_port_pll_define_nof_plls_per_pm,
    dnxf_data_port_pll_define_nof_config_words,

    
    _dnxf_data_port_pll_define_nof
} dnxf_data_port_pll_define_e;



uint32 dnxf_data_port_pll_nof_lcpll_get(
    int unit);


uint32 dnxf_data_port_pll_nof_port_in_lcpll_get(
    int unit);


uint32 dnxf_data_port_pll_fabric_clock_freq_in_default_get(
    int unit);


uint32 dnxf_data_port_pll_fabric_clock_freq_out_default_get(
    int unit);


uint32 dnxf_data_port_pll_nof_plls_per_pm_get(
    int unit);


uint32 dnxf_data_port_pll_nof_config_words_get(
    int unit);



typedef enum
{
    dnxf_data_port_pll_table_info,
    dnxf_data_port_pll_table_force_single_pll,
    dnxf_data_port_pll_table_config,

    
    _dnxf_data_port_pll_table_nof
} dnxf_data_port_pll_table_e;



const dnxf_data_port_pll_info_t * dnxf_data_port_pll_info_get(
    int unit,
    int pll);


const dnxf_data_port_pll_force_single_pll_t * dnxf_data_port_pll_force_single_pll_get(
    int unit,
    int pm);


const dnxf_data_port_pll_config_t * dnxf_data_port_pll_config_get(
    int unit,
    int fabric_ref_clk_in,
    int fabric_ref_clk_out);



shr_error_e dnxf_data_port_pll_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_pll_force_single_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_pll_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_pll_info_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_pll_force_single_pll_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_pll_config_info_get(
    int unit);






int dnxf_data_port_synce_feature_get(
    int unit,
    dnxf_data_port_synce_feature_e feature);



typedef enum
{
    dnxf_data_port_synce_define_fabric_div_min,
    dnxf_data_port_synce_define_fabric_div_max,

    
    _dnxf_data_port_synce_define_nof
} dnxf_data_port_synce_define_e;



uint32 dnxf_data_port_synce_fabric_div_min_get(
    int unit);


uint32 dnxf_data_port_synce_fabric_div_max_get(
    int unit);



typedef enum
{
    dnxf_data_port_synce_table_cfg,
    dnxf_data_port_synce_table_pm_to_ipw_mapping,
    dnxf_data_port_synce_table_ipw_info,

    
    _dnxf_data_port_synce_table_nof
} dnxf_data_port_synce_table_e;



const dnxf_data_port_synce_cfg_t * dnxf_data_port_synce_cfg_get(
    int unit,
    int synce_index);


const dnxf_data_port_synce_pm_to_ipw_mapping_t * dnxf_data_port_synce_pm_to_ipw_mapping_get(
    int unit,
    int pm_idx);


const dnxf_data_port_synce_ipw_info_t * dnxf_data_port_synce_ipw_info_get(
    int unit,
    int ipw);



shr_error_e dnxf_data_port_synce_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_synce_pm_to_ipw_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_port_synce_ipw_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_synce_cfg_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_synce_pm_to_ipw_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_port_synce_ipw_info_info_get(
    int unit);






int dnxf_data_port_retimer_feature_get(
    int unit,
    dnxf_data_port_retimer_feature_e feature);



typedef enum
{

    
    _dnxf_data_port_retimer_define_nof
} dnxf_data_port_retimer_define_e;




typedef enum
{
    dnxf_data_port_retimer_table_links_connection,

    
    _dnxf_data_port_retimer_table_nof
} dnxf_data_port_retimer_table_e;



const dnxf_data_port_retimer_links_connection_t * dnxf_data_port_retimer_links_connection_get(
    int unit,
    int link);



shr_error_e dnxf_data_port_retimer_links_connection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_port_retimer_links_connection_info_get(
    int unit);






int dnxf_data_port_regression_feature_get(
    int unit,
    dnxf_data_port_regression_feature_e feature);



typedef enum
{

    
    _dnxf_data_port_regression_define_nof
} dnxf_data_port_regression_define_e;




typedef enum
{

    
    _dnxf_data_port_regression_table_nof
} dnxf_data_port_regression_table_e;






shr_error_e dnxf_data_port_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

