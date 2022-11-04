
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_PORT_H_

#define _DNXF_DATA_PORT_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <bcm/types.h>
#include <shared/pbmp.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_port_init(
    int unit);






typedef struct
{
    bcm_pbmp_t pbmp;
} dnxf_data_port_general_supported_lanes_t;


typedef struct
{
    int size;
} dnxf_data_port_general_fmac_bus_size_t;


typedef struct
{
    uint32 speed;
    bcm_port_phy_fec_t fec_type;
    uint32 is_valid;
} dnxf_data_port_general_supported_interfaces_t;


typedef struct
{
    bcm_port_phy_fec_t fec_type;
} dnxf_data_port_general_default_fec_t;


typedef struct
{
    bcm_port_phy_fec_t fec_type;
    bcm_port_control_t control_type;
    uint32 is_valid;
} dnxf_data_port_general_supported_fec_controls_t;


typedef struct
{
    uint32 size;
} dnxf_data_port_general_supported_fmac_tx_padding_sizes_t;


typedef struct
{
    uint32 counted_bits;
    uint32 total_bits;
    uint32 max_nof_correctable_symbols;
    uint32 is_valid;
} dnxf_data_port_general_fec_compensation_info_t;


typedef struct
{
    uint8 port_disable_required;
} dnxf_data_port_general_phy_controls_features_t;



typedef enum
{
    dnxf_data_port_general_mac_cell_shaper_supported,
    dnxf_data_port_general_bw_update,
    dnxf_data_port_general_is_cable_swap_supported,
    dnxf_data_port_general_linkup_indication_map_supported,
    dnxf_data_port_general_is_connection_to_repeater_supported,
    dnxf_data_port_general_fmac_tx_padding_supported,
    dnxf_data_port_general_link_local_remote_fault_support,
    dnxf_data_port_general_fmac_only_disable_supported,
    dnxf_data_port_general_max_speed_api_supported,
    dnxf_data_port_general_fec_ber_proj_supported,
    dnxf_data_port_general_port_selective_set_api_supported,
    dnxf_data_port_general_serdes_lpi_access_supported,
    dnxf_data_port_general_is_phy_rx_taps_supported,
    dnxf_data_port_general_is_serdes_tx_soc_prop_dict_format_supported,
    dnxf_data_port_general_is_signal_quality_supported,
    dnxf_data_port_general_is_scrambling_disable_supported,

    
    _dnxf_data_port_general_feature_nof
} dnxf_data_port_general_feature_e;



typedef int(
    *dnxf_data_port_general_feature_get_f) (
    int unit,
    dnxf_data_port_general_feature_e feature);


typedef uint32(
    *dnxf_data_port_general_nof_links_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_nof_pms_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_max_bucket_fill_rate_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_link_mask_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_aldwp_max_val_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_fmac_clock_khz_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_speed_max_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_fmac_tx_padding_min_port_speed_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_nof_ber_proj_error_analyzer_counters_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_fw_load_method_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_fw_crc_check_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_fw_load_verify_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_general_mac_bucket_fill_rate_get_f) (
    int unit);


typedef const dnxf_data_port_general_supported_lanes_t *(
    *dnxf_data_port_general_supported_lanes_get_f) (
    int unit);


typedef const dnxf_data_port_general_fmac_bus_size_t *(
    *dnxf_data_port_general_fmac_bus_size_get_f) (
    int unit,
    int mode);


typedef const dnxf_data_port_general_supported_interfaces_t *(
    *dnxf_data_port_general_supported_interfaces_get_f) (
    int unit,
    int index);


typedef const dnxf_data_port_general_default_fec_t *(
    *dnxf_data_port_general_default_fec_get_f) (
    int unit,
    int speed);


typedef const dnxf_data_port_general_supported_fec_controls_t *(
    *dnxf_data_port_general_supported_fec_controls_get_f) (
    int unit,
    int index);


typedef const dnxf_data_port_general_supported_fmac_tx_padding_sizes_t *(
    *dnxf_data_port_general_supported_fmac_tx_padding_sizes_get_f) (
    int unit,
    int index);


typedef const dnxf_data_port_general_fec_compensation_info_t *(
    *dnxf_data_port_general_fec_compensation_info_get_f) (
    int unit,
    int fec_type);


typedef const dnxf_data_port_general_phy_controls_features_t *(
    *dnxf_data_port_general_phy_controls_features_get_f) (
    int unit,
    int phy_control_type);



typedef struct
{
    
    dnxf_data_port_general_feature_get_f feature_get;
    
    dnxf_data_port_general_nof_links_get_f nof_links_get;
    
    dnxf_data_port_general_nof_pms_get_f nof_pms_get;
    
    dnxf_data_port_general_max_bucket_fill_rate_get_f max_bucket_fill_rate_get;
    
    dnxf_data_port_general_link_mask_get_f link_mask_get;
    
    dnxf_data_port_general_aldwp_max_val_get_f aldwp_max_val_get;
    
    dnxf_data_port_general_fmac_clock_khz_get_f fmac_clock_khz_get;
    
    dnxf_data_port_general_speed_max_get_f speed_max_get;
    
    dnxf_data_port_general_fmac_tx_padding_min_port_speed_get_f fmac_tx_padding_min_port_speed_get;
    
    dnxf_data_port_general_nof_ber_proj_error_analyzer_counters_get_f nof_ber_proj_error_analyzer_counters_get;
    
    dnxf_data_port_general_fw_load_method_get_f fw_load_method_get;
    
    dnxf_data_port_general_fw_crc_check_get_f fw_crc_check_get;
    
    dnxf_data_port_general_fw_load_verify_get_f fw_load_verify_get;
    
    dnxf_data_port_general_mac_bucket_fill_rate_get_f mac_bucket_fill_rate_get;
    
    dnxf_data_port_general_supported_lanes_get_f supported_lanes_get;
    
    dnxc_data_table_info_get_f supported_lanes_info_get;
    
    dnxf_data_port_general_fmac_bus_size_get_f fmac_bus_size_get;
    
    dnxc_data_table_info_get_f fmac_bus_size_info_get;
    
    dnxf_data_port_general_supported_interfaces_get_f supported_interfaces_get;
    
    dnxc_data_table_info_get_f supported_interfaces_info_get;
    
    dnxf_data_port_general_default_fec_get_f default_fec_get;
    
    dnxc_data_table_info_get_f default_fec_info_get;
    
    dnxf_data_port_general_supported_fec_controls_get_f supported_fec_controls_get;
    
    dnxc_data_table_info_get_f supported_fec_controls_info_get;
    
    dnxf_data_port_general_supported_fmac_tx_padding_sizes_get_f supported_fmac_tx_padding_sizes_get;
    
    dnxc_data_table_info_get_f supported_fmac_tx_padding_sizes_info_get;
    
    dnxf_data_port_general_fec_compensation_info_get_f fec_compensation_info_get;
    
    dnxc_data_table_info_get_f fec_compensation_info_info_get;
    
    dnxf_data_port_general_phy_controls_features_get_f phy_controls_features_get;
    
    dnxc_data_table_info_get_f phy_controls_features_info_get;
} dnxf_data_if_port_general_t;







typedef enum
{

    
    _dnxf_data_port_stat_feature_nof
} dnxf_data_port_stat_feature_e;



typedef int(
    *dnxf_data_port_stat_feature_get_f) (
    int unit,
    dnxf_data_port_stat_feature_e feature);


typedef uint32(
    *dnxf_data_port_stat_thread_interval_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_stat_thread_flags_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_stat_thread_counter_pri_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_stat_thread_timeout_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_port_stat_feature_get_f feature_get;
    
    dnxf_data_port_stat_thread_interval_get_f thread_interval_get;
    
    dnxf_data_port_stat_thread_flags_get_f thread_flags_get;
    
    dnxf_data_port_stat_thread_counter_pri_get_f thread_counter_pri_get;
    
    dnxf_data_port_stat_thread_timeout_get_f thread_timeout_get;
} dnxf_data_if_port_stat_t;






typedef struct
{
    int speed;
    uint32 link_training;
    bcm_port_phy_fec_t fec_type;
    uint32 tx_polarity;
    uint32 rx_polarity;
    uint32 connected_to_repeater;
    uint32 tx_pam4_precoder;
    uint32 lp_tx_precoder;
    uint32 cell_format;
} dnxf_data_port_static_add_port_info_t;


typedef struct
{
    soc_dnxc_port_dfe_mode_t dfe;
    int media_type;
    int unreliable_los;
    int cl72_auto_polarity_enable;
    int cl72_restart_timeout_enable;
    soc_dnxc_port_channel_mode_t channel_mode;
} dnxf_data_port_static_add_serdes_lane_config_t;


typedef struct
{
    int pre3;
    int pre2;
    int pre;
    int main;
    int post;
    int post2;
    int post3;
    bcm_port_phy_tx_tap_mode_t tx_tap_mode;
    bcm_port_phy_signalling_mode_t signalling_mode;
} dnxf_data_port_static_add_serdes_tx_taps_t;


typedef struct
{
    int quad_enable;
} dnxf_data_port_static_add_quad_info_t;


typedef struct
{
    int enable;
    int master;
} dnxf_data_port_static_add_cable_swap_info_t;



typedef enum
{
    dnxf_data_port_static_add_serdes_tx_taps_pre3_supported_instead_post3,

    
    _dnxf_data_port_static_add_feature_nof
} dnxf_data_port_static_add_feature_e;



typedef int(
    *dnxf_data_port_static_add_feature_get_f) (
    int unit,
    dnxf_data_port_static_add_feature_e feature);


typedef const dnxf_data_port_static_add_port_info_t *(
    *dnxf_data_port_static_add_port_info_get_f) (
    int unit,
    int port);


typedef const dnxf_data_port_static_add_serdes_lane_config_t *(
    *dnxf_data_port_static_add_serdes_lane_config_get_f) (
    int unit,
    int port);


typedef const dnxf_data_port_static_add_serdes_tx_taps_t *(
    *dnxf_data_port_static_add_serdes_tx_taps_get_f) (
    int unit,
    int port);


typedef const dnxf_data_port_static_add_quad_info_t *(
    *dnxf_data_port_static_add_quad_info_get_f) (
    int unit,
    int quad);


typedef const dnxf_data_port_static_add_cable_swap_info_t *(
    *dnxf_data_port_static_add_cable_swap_info_get_f) (
    int unit,
    int pm);



typedef struct
{
    
    dnxf_data_port_static_add_feature_get_f feature_get;
    
    dnxf_data_port_static_add_port_info_get_f port_info_get;
    
    dnxc_data_table_info_get_f port_info_info_get;
    
    dnxf_data_port_static_add_serdes_lane_config_get_f serdes_lane_config_get;
    
    dnxc_data_table_info_get_f serdes_lane_config_info_get;
    
    dnxf_data_port_static_add_serdes_tx_taps_get_f serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f serdes_tx_taps_info_get;
    
    dnxf_data_port_static_add_quad_info_get_f quad_info_get;
    
    dnxc_data_table_info_get_f quad_info_info_get;
    
    dnxf_data_port_static_add_cable_swap_info_get_f cable_swap_info_get;
    
    dnxc_data_table_info_get_f cable_swap_info_info_get;
} dnxf_data_if_port_static_add_t;






typedef struct
{
    uint32 serdes_rx_id;
    uint32 serdes_tx_id;
} dnxf_data_port_lane_map_info_t;



typedef enum
{
    dnxf_data_port_lane_map_is_flexible_link_mapping_supported,
    dnxf_data_port_lane_map_is_full_fsrd_mapping_range_supported,

    
    _dnxf_data_port_lane_map_feature_nof
} dnxf_data_port_lane_map_feature_e;



typedef int(
    *dnxf_data_port_lane_map_feature_get_f) (
    int unit,
    dnxf_data_port_lane_map_feature_e feature);


typedef uint32(
    *dnxf_data_port_lane_map_lane_map_bound_get_f) (
    int unit);


typedef const dnxf_data_port_lane_map_info_t *(
    *dnxf_data_port_lane_map_info_get_f) (
    int unit,
    int lane);



typedef struct
{
    
    dnxf_data_port_lane_map_feature_get_f feature_get;
    
    dnxf_data_port_lane_map_lane_map_bound_get_f lane_map_bound_get;
    
    dnxf_data_port_lane_map_info_get_f info_get;
    
    dnxc_data_table_info_get_f info_info_get;
} dnxf_data_if_port_lane_map_t;






typedef struct
{
    uint32 fabric_ref_clk_in;
    uint32 fabric_ref_clk_out;
} dnxf_data_port_pll_info_t;


typedef struct
{
    int enable;
} dnxf_data_port_pll_force_single_pll_t;


typedef struct
{
    uint32 data[DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS];
    uint32 valid;
} dnxf_data_port_pll_config_t;



typedef enum
{

    
    _dnxf_data_port_pll_feature_nof
} dnxf_data_port_pll_feature_e;



typedef int(
    *dnxf_data_port_pll_feature_get_f) (
    int unit,
    dnxf_data_port_pll_feature_e feature);


typedef uint32(
    *dnxf_data_port_pll_nof_lcpll_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_pll_nof_port_in_lcpll_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_pll_fabric_clock_freq_in_default_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_pll_fabric_clock_freq_out_default_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_pll_nof_plls_per_pm_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_pll_nof_config_words_get_f) (
    int unit);


typedef const dnxf_data_port_pll_info_t *(
    *dnxf_data_port_pll_info_get_f) (
    int unit,
    int pll);


typedef const dnxf_data_port_pll_force_single_pll_t *(
    *dnxf_data_port_pll_force_single_pll_get_f) (
    int unit,
    int pm);


typedef const dnxf_data_port_pll_config_t *(
    *dnxf_data_port_pll_config_get_f) (
    int unit,
    int fabric_ref_clk_in,
    int fabric_ref_clk_out);



typedef struct
{
    
    dnxf_data_port_pll_feature_get_f feature_get;
    
    dnxf_data_port_pll_nof_lcpll_get_f nof_lcpll_get;
    
    dnxf_data_port_pll_nof_port_in_lcpll_get_f nof_port_in_lcpll_get;
    
    dnxf_data_port_pll_fabric_clock_freq_in_default_get_f fabric_clock_freq_in_default_get;
    
    dnxf_data_port_pll_fabric_clock_freq_out_default_get_f fabric_clock_freq_out_default_get;
    
    dnxf_data_port_pll_nof_plls_per_pm_get_f nof_plls_per_pm_get;
    
    dnxf_data_port_pll_nof_config_words_get_f nof_config_words_get;
    
    dnxf_data_port_pll_info_get_f info_get;
    
    dnxc_data_table_info_get_f info_info_get;
    
    dnxf_data_port_pll_force_single_pll_get_f force_single_pll_get;
    
    dnxc_data_table_info_get_f force_single_pll_info_get;
    
    dnxf_data_port_pll_config_get_f config_get;
    
    dnxc_data_table_info_get_f config_info_get;
} dnxf_data_if_port_pll_t;






typedef struct
{
    int source_clock_port;
    uint32 source_clock_divider;
} dnxf_data_port_synce_cfg_t;


typedef struct
{
    uint32 ipw;
} dnxf_data_port_synce_pm_to_ipw_mapping_t;


typedef struct
{
    uint32 first_pm;
} dnxf_data_port_synce_ipw_info_t;



typedef enum
{
    dnxf_data_port_synce_multi_set_is_supported,

    
    _dnxf_data_port_synce_feature_nof
} dnxf_data_port_synce_feature_e;



typedef int(
    *dnxf_data_port_synce_feature_get_f) (
    int unit,
    dnxf_data_port_synce_feature_e feature);


typedef uint32(
    *dnxf_data_port_synce_fabric_div_min_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_synce_fabric_div_max_get_f) (
    int unit);


typedef const dnxf_data_port_synce_cfg_t *(
    *dnxf_data_port_synce_cfg_get_f) (
    int unit,
    int synce_index);


typedef const dnxf_data_port_synce_pm_to_ipw_mapping_t *(
    *dnxf_data_port_synce_pm_to_ipw_mapping_get_f) (
    int unit,
    int pm_idx);


typedef const dnxf_data_port_synce_ipw_info_t *(
    *dnxf_data_port_synce_ipw_info_get_f) (
    int unit,
    int ipw);



typedef struct
{
    
    dnxf_data_port_synce_feature_get_f feature_get;
    
    dnxf_data_port_synce_fabric_div_min_get_f fabric_div_min_get;
    
    dnxf_data_port_synce_fabric_div_max_get_f fabric_div_max_get;
    
    dnxf_data_port_synce_cfg_get_f cfg_get;
    
    dnxc_data_table_info_get_f cfg_info_get;
    
    dnxf_data_port_synce_pm_to_ipw_mapping_get_f pm_to_ipw_mapping_get;
    
    dnxc_data_table_info_get_f pm_to_ipw_mapping_info_get;
    
    dnxf_data_port_synce_ipw_info_get_f ipw_info_get;
    
    dnxc_data_table_info_get_f ipw_info_info_get;
} dnxf_data_if_port_synce_t;






typedef struct
{
    soc_port_t connected_link;
} dnxf_data_port_retimer_links_connection_t;



typedef enum
{
    dnxf_data_port_retimer_is_supported,

    
    _dnxf_data_port_retimer_feature_nof
} dnxf_data_port_retimer_feature_e;



typedef int(
    *dnxf_data_port_retimer_feature_get_f) (
    int unit,
    dnxf_data_port_retimer_feature_e feature);


typedef const dnxf_data_port_retimer_links_connection_t *(
    *dnxf_data_port_retimer_links_connection_get_f) (
    int unit,
    int link);



typedef struct
{
    
    dnxf_data_port_retimer_feature_get_f feature_get;
    
    dnxf_data_port_retimer_links_connection_get_f links_connection_get;
    
    dnxc_data_table_info_get_f links_connection_info_get;
} dnxf_data_if_port_retimer_t;







typedef enum
{
    dnxf_data_port_regression_is_cell_format_set_supported,
    dnxf_data_port_regression_is_power_measurement_supported,
    dnxf_data_port_regression_is_hist_errcnt_thresh_used,

    
    _dnxf_data_port_regression_feature_nof
} dnxf_data_port_regression_feature_e;



typedef int(
    *dnxf_data_port_regression_feature_get_f) (
    int unit,
    dnxf_data_port_regression_feature_e feature);



typedef struct
{
    
    dnxf_data_port_regression_feature_get_f feature_get;
} dnxf_data_if_port_regression_t;





typedef struct
{
    
    dnxf_data_if_port_general_t general;
    
    dnxf_data_if_port_stat_t stat;
    
    dnxf_data_if_port_static_add_t static_add;
    
    dnxf_data_if_port_lane_map_t lane_map;
    
    dnxf_data_if_port_pll_t pll;
    
    dnxf_data_if_port_synce_t synce;
    
    dnxf_data_if_port_retimer_t retimer;
    
    dnxf_data_if_port_regression_t regression;
} dnxf_data_if_port_t;




extern dnxf_data_if_port_t dnxf_data_port;


#endif 

