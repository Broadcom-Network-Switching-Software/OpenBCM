

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_PORT_H_

#define _DNXF_DATA_PORT_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <bcm/types.h>
#include <shared/pbmp.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_port_init(
    int unit);






typedef struct
{
    
    bcm_pbmp_t pbmp;
} dnxf_data_port_general_supported_phys_t;


typedef struct
{
    
    int size;
} dnxf_data_port_general_fmac_bus_size_t;



typedef enum
{
    
    dnxf_data_port_general_mac_cell_shaper_supported,

    
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


typedef const dnxf_data_port_general_supported_phys_t *(
    *dnxf_data_port_general_supported_phys_get_f) (
    int unit);


typedef const dnxf_data_port_general_fmac_bus_size_t *(
    *dnxf_data_port_general_fmac_bus_size_get_f) (
    int unit,
    int mode);



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
    
    dnxf_data_port_general_fw_load_method_get_f fw_load_method_get;
    
    dnxf_data_port_general_fw_crc_check_get_f fw_crc_check_get;
    
    dnxf_data_port_general_fw_load_verify_get_f fw_load_verify_get;
    
    dnxf_data_port_general_mac_bucket_fill_rate_get_f mac_bucket_fill_rate_get;
    
    dnxf_data_port_general_supported_phys_get_f supported_phys_get;
    
    dnxc_data_table_info_get_f supported_phys_info_get;
    
    dnxf_data_port_general_fmac_bus_size_get_f fmac_bus_size_get;
    
    dnxc_data_table_info_get_f fmac_bus_size_info_get;
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
    *dnxf_data_port_stat_thread_counter_pri_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_port_stat_thread_timeout_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_port_stat_feature_get_f feature_get;
    
    dnxf_data_port_stat_thread_interval_get_f thread_interval_get;
    
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



typedef enum
{

    
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
} dnxf_data_if_port_static_add_t;






typedef struct
{
    
    uint32 serdes_rx_id;
    
    uint32 serdes_tx_id;
} dnxf_data_port_lane_map_info_t;



typedef enum
{

    
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
    
    dnxf_data_port_pll_config_get_f config_get;
    
    dnxc_data_table_info_get_f config_info_get;
} dnxf_data_if_port_pll_t;






typedef struct
{
    
    int source_clock_port;
    
    uint32 source_clock_divider;
} dnxf_data_port_synce_cfg_t;



typedef enum
{

    
    _dnxf_data_port_synce_feature_nof
} dnxf_data_port_synce_feature_e;



typedef int(
    *dnxf_data_port_synce_feature_get_f) (
    int unit,
    dnxf_data_port_synce_feature_e feature);


typedef const dnxf_data_port_synce_cfg_t *(
    *dnxf_data_port_synce_cfg_get_f) (
    int unit,
    int synce_index);



typedef struct
{
    
    dnxf_data_port_synce_feature_get_f feature_get;
    
    dnxf_data_port_synce_cfg_get_f cfg_get;
    
    dnxc_data_table_info_get_f cfg_info_get;
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
    dnxf_data_port_features_isolation_needed_before_disable,

    
    _dnxf_data_port_features_feature_nof
} dnxf_data_port_features_feature_e;



typedef int(
    *dnxf_data_port_features_feature_get_f) (
    int unit,
    dnxf_data_port_features_feature_e feature);



typedef struct
{
    
    dnxf_data_port_features_feature_get_f feature_get;
} dnxf_data_if_port_features_t;





typedef struct
{
    
    dnxf_data_if_port_general_t general;
    
    dnxf_data_if_port_stat_t stat;
    
    dnxf_data_if_port_static_add_t static_add;
    
    dnxf_data_if_port_lane_map_t lane_map;
    
    dnxf_data_if_port_pll_t pll;
    
    dnxf_data_if_port_synce_t synce;
    
    dnxf_data_if_port_retimer_t retimer;
    
    dnxf_data_if_port_features_t features;
} dnxf_data_if_port_t;




extern dnxf_data_if_port_t dnxf_data_port;


#endif 

