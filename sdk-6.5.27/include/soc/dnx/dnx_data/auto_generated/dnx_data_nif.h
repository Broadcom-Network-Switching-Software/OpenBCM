
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_NIF_H_

#define _DNX_DATA_NIF_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_nif_init(
    int unit);






typedef struct
{
    uint32 min_phy_id;
    uint32 max_phy_id;
} dnx_data_nif_global_pll_phys_t;


typedef struct
{
    uint32 unit_id;
    uint32 internal_offset;
} dnx_data_nif_global_nif_interface_id_to_unit_id_t;



typedef enum
{
    dnx_data_nif_global_not_implemented,
    dnx_data_nif_global_reassembler,
    dnx_data_nif_global_clock_power_down,
    dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down,
    dnx_data_nif_global_tx_separate_clock_power_down,
    dnx_data_nif_global_ilkn_credit_loss_handling_mode,
    dnx_data_nif_global_bh_refclk_termination_inverted,
    dnx_data_nif_global_is_shared_arch,
    dnx_data_nif_global_is_ilkn_after_flexe,
    dnx_data_nif_global_mgmt_port_supported,
    dnx_data_nif_global_support_xpmd_if,
    dnx_data_nif_global_is_g_hao_supported,
    dnx_data_nif_global_bh_2x2_supported,
    dnx_data_nif_global_eth_use_framer_mgmt_bw,
    dnx_data_nif_global_framer_phy_hw_linkscan_supported,

    
    _dnx_data_nif_global_feature_nof
} dnx_data_nif_global_feature_e;



typedef int(
    *dnx_data_nif_global_feature_get_f) (
    int unit,
    dnx_data_nif_global_feature_e feature);


typedef uint32(
    *dnx_data_nif_global_nof_lcplls_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_nof_nif_interfaces_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_nof_nif_units_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_reassembler_fifo_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_total_data_capacity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_single_mgmt_port_capacity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_max_nof_lanes_per_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_ghao_speed_limit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_start_tx_threshold_global_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_l1_only_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_otn_supported_get_f) (
    int unit);


typedef const dnx_data_nif_global_pll_phys_t *(
    *dnx_data_nif_global_pll_phys_get_f) (
    int unit,
    int lcpll);


typedef const dnx_data_nif_global_nif_interface_id_to_unit_id_t *(
    *dnx_data_nif_global_nif_interface_id_to_unit_id_get_f) (
    int unit,
    int nif_interface_id);



typedef struct
{
    
    dnx_data_nif_global_feature_get_f feature_get;
    
    dnx_data_nif_global_nof_lcplls_get_f nof_lcplls_get;
    
    dnx_data_nif_global_nof_nif_interfaces_per_core_get_f nof_nif_interfaces_per_core_get;
    
    dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get_f nif_interface_id_to_unit_id_granularity_get;
    
    dnx_data_nif_global_nof_nif_units_per_core_get_f nof_nif_units_per_core_get;
    
    dnx_data_nif_global_reassembler_fifo_threshold_get_f reassembler_fifo_threshold_get;
    
    dnx_data_nif_global_total_data_capacity_get_f total_data_capacity_get;
    
    dnx_data_nif_global_single_mgmt_port_capacity_get_f single_mgmt_port_capacity_get;
    
    dnx_data_nif_global_max_nof_lanes_per_port_get_f max_nof_lanes_per_port_get;
    
    dnx_data_nif_global_ghao_speed_limit_get_f ghao_speed_limit_get;
    
    dnx_data_nif_global_start_tx_threshold_global_get_f start_tx_threshold_global_get;
    
    dnx_data_nif_global_l1_only_mode_get_f l1_only_mode_get;
    
    dnx_data_nif_global_otn_supported_get_f otn_supported_get;
    
    dnx_data_nif_global_pll_phys_get_f pll_phys_get;
    
    dnxc_data_table_info_get_f pll_phys_info_get;
    
    dnx_data_nif_global_nif_interface_id_to_unit_id_get_f nif_interface_id_to_unit_id_get;
    
    dnxc_data_table_info_get_f nif_interface_id_to_unit_id_info_get;
} dnx_data_if_nif_global_t;






typedef struct
{
    bcm_pbmp_t supported_phys;
} dnx_data_nif_phys_general_t;


typedef struct
{
    uint32 tx_polarity;
    uint32 rx_polarity;
} dnx_data_nif_phys_polarity_t;


typedef struct
{
    bcm_pbmp_t phys;
} dnx_data_nif_phys_core_phys_map_t;


typedef struct
{
    uint32 vco_div;
} dnx_data_nif_phys_vco_div_t;


typedef struct
{
    uint32 nof_phys;
    uint32 first_phy;
} dnx_data_nif_phys_nof_phys_per_core_t;


typedef struct
{
    int speed;
    int val;
} dnx_data_nif_phys_txpi_sdm_div_t;



typedef enum
{
    dnx_data_nif_phys_txpi_supported_in_pm8x50,

    
    _dnx_data_nif_phys_feature_nof
} dnx_data_nif_phys_feature_e;



typedef int(
    *dnx_data_nif_phys_feature_get_f) (
    int unit,
    dnx_data_nif_phys_feature_e feature);


typedef uint32(
    *dnx_data_nif_phys_nof_phys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_max_phys_in_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_pm8x50_gen_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_is_pam4_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_first_mgmt_phy_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_nof_mgmt_phys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_txpi_sdm_scheme_get_f) (
    int unit);


typedef const dnx_data_nif_phys_general_t *(
    *dnx_data_nif_phys_general_get_f) (
    int unit);


typedef const dnx_data_nif_phys_polarity_t *(
    *dnx_data_nif_phys_polarity_get_f) (
    int unit,
    int lane_index);


typedef const dnx_data_nif_phys_core_phys_map_t *(
    *dnx_data_nif_phys_core_phys_map_get_f) (
    int unit,
    int core_index);


typedef const dnx_data_nif_phys_vco_div_t *(
    *dnx_data_nif_phys_vco_div_get_f) (
    int unit,
    int ethu_index);


typedef const dnx_data_nif_phys_nof_phys_per_core_t *(
    *dnx_data_nif_phys_nof_phys_per_core_get_f) (
    int unit,
    int core_index);


typedef const dnx_data_nif_phys_txpi_sdm_div_t *(
    *dnx_data_nif_phys_txpi_sdm_div_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_nif_phys_feature_get_f feature_get;
    
    dnx_data_nif_phys_nof_phys_get_f nof_phys_get;
    
    dnx_data_nif_phys_max_phys_in_core_get_f max_phys_in_core_get;
    
    dnx_data_nif_phys_pm8x50_gen_get_f pm8x50_gen_get;
    
    dnx_data_nif_phys_is_pam4_speed_supported_get_f is_pam4_speed_supported_get;
    
    dnx_data_nif_phys_first_mgmt_phy_get_f first_mgmt_phy_get;
    
    dnx_data_nif_phys_nof_mgmt_phys_get_f nof_mgmt_phys_get;
    
    dnx_data_nif_phys_txpi_sdm_scheme_get_f txpi_sdm_scheme_get;
    
    dnx_data_nif_phys_general_get_f general_get;
    
    dnxc_data_table_info_get_f general_info_get;
    
    dnx_data_nif_phys_polarity_get_f polarity_get;
    
    dnxc_data_table_info_get_f polarity_info_get;
    
    dnx_data_nif_phys_core_phys_map_get_f core_phys_map_get;
    
    dnxc_data_table_info_get_f core_phys_map_info_get;
    
    dnx_data_nif_phys_vco_div_get_f vco_div_get;
    
    dnxc_data_table_info_get_f vco_div_info_get;
    
    dnx_data_nif_phys_nof_phys_per_core_get_f nof_phys_per_core_get;
    
    dnxc_data_table_info_get_f nof_phys_per_core_info_get;
    
    dnx_data_nif_phys_txpi_sdm_div_get_f txpi_sdm_div_get;
    
    dnxc_data_table_info_get_f txpi_sdm_div_info_get;
} dnx_data_if_nif_phys_t;






typedef struct
{
    bcm_pbmp_t bitmap;
    uint32 lanes_array[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    uint8 array_order_valid;
} dnx_data_nif_ilkn_phys_t;


typedef struct
{
    uint32 is_supported;
    uint32 max_phys;
    bcm_pbmp_t nif_phys;
    bcm_pbmp_t fabric_phys;
} dnx_data_nif_ilkn_supported_phys_t;


typedef struct
{
    uint32 speed;
    portmod_dispatch_type_t pm_dispatch_type;
    bcm_port_phy_fec_t fec_type;
    uint32 is_valid;
} dnx_data_nif_ilkn_supported_interfaces_t;


typedef struct
{
    uint32 is_elk_supported;
    uint32 is_data_supported;
    uint32 fec_disable_by_bypass;
    uint32 fec_dependency;
} dnx_data_nif_ilkn_ilkn_cores_t;


typedef struct
{
    uint32 clu_unit;
} dnx_data_nif_ilkn_ilkn_cores_clup_facing_t;


typedef struct
{
    int core;
} dnx_data_nif_ilkn_supported_device_core_t;


typedef struct
{
    uint32 burst_short;
    uint32 burst_max;
    uint32 burst_min;
    uint32 metaframe_period;
    uint32 is_over_fabric;
} dnx_data_nif_ilkn_properties_t;


typedef struct
{
    portmod_dispatch_type_t dispatch_type;
    uint32 first_phy;
    uint32 nof_phys;
    int facing_core_index;
} dnx_data_nif_ilkn_nif_pms_t;


typedef struct
{
    portmod_dispatch_type_t dispatch_type;
    uint32 first_phy;
    uint32 nof_phys;
    int facing_core_index;
} dnx_data_nif_ilkn_fabric_pms_t;


typedef struct
{
    uint32 first_phy;
    uint32 ilkn_lane_offset;
    uint32 nof_lanes;
} dnx_data_nif_ilkn_nif_lanes_map_t;


typedef struct
{
    uint32 first_phy;
    uint32 ilkn_lane_offset;
    uint32 nof_lanes;
} dnx_data_nif_ilkn_fabric_lanes_map_t;


typedef struct
{
    uint32 speed;
    uint32 start_thr;
} dnx_data_nif_ilkn_start_tx_threshold_table_t;


typedef struct
{
    uint32 first_phy;
    uint32 last_phy;
} dnx_data_nif_ilkn_connectivity_options_t;



typedef enum
{
    dnx_data_nif_ilkn_is_supported,
    dnx_data_nif_ilkn_is_ilu_supported,
    dnx_data_nif_ilkn_is_elk_supported,
    dnx_data_nif_ilkn_credits_selector,
    dnx_data_nif_ilkn_over_fabric_per_core_selector,
    dnx_data_nif_ilkn_cdu_ilkn_selectors,
    dnx_data_nif_ilkn_clu_ilkn_selectors,
    dnx_data_nif_ilkn_clu_ilkn_direction_selectors,
    dnx_data_nif_ilkn_frmr_ilkn_selector,
    dnx_data_nif_ilkn_support_burst_interleaving,
    dnx_data_nif_ilkn_support_aligner,
    dnx_data_nif_ilkn_scheduler_context_per_port_support,
    dnx_data_nif_ilkn_support_ilu_burst_max,
    dnx_data_nif_ilkn_is_fec_supported,
    dnx_data_nif_ilkn_is_fec_bypass_supported,
    dnx_data_nif_ilkn_fec_units_in_cdu,
    dnx_data_nif_ilkn_hrf_flush_support,
    dnx_data_nif_ilkn_per_port_segmentation_hrf_reset,
    dnx_data_nif_ilkn_elk_require_core_reset_after_link_training,
    dnx_data_nif_ilkn_reset_core_after_link_training,
    dnx_data_nif_ilkn_ilkn_over_fabric_only,
    dnx_data_nif_ilkn_12lanes_mode,
    dnx_data_nif_ilkn_legacy_lane_format,
    dnx_data_nif_ilkn_is_data_path_through_w40,
    dnx_data_nif_ilkn_ensure_tx_rx_reset_order,
    dnx_data_nif_ilkn_is_nif_56G_speed_supported,
    dnx_data_nif_ilkn_is_fabric_56G_speed_supported,
    dnx_data_nif_ilkn_is_10p3G_speed_supported,
    dnx_data_nif_ilkn_is_12p5G_speed_supported,
    dnx_data_nif_ilkn_is_20p6G_speed_supported,
    dnx_data_nif_ilkn_is_25G_speed_supported,
    dnx_data_nif_ilkn_is_25p7G_speed_supported,
    dnx_data_nif_ilkn_is_27p3G_speed_supported,
    dnx_data_nif_ilkn_is_28p1G_speed_supported,
    dnx_data_nif_ilkn_is_53p1G_speed_supported,
    dnx_data_nif_ilkn_is_56p2G_speed_supported,

    
    _dnx_data_nif_ilkn_feature_nof
} dnx_data_nif_ilkn_feature_e;



typedef int(
    *dnx_data_nif_ilkn_feature_get_f) (
    int unit,
    dnx_data_nif_ilkn_feature_e feature);


typedef uint32(
    *dnx_data_nif_ilkn_ilu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_unit_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_unit_if_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_if_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_fec_units_per_core_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_nof_lanes_per_fec_unit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_lanes_max_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_lanes_max_nof_using_fec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_lanes_allowed_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_over_eth_pms_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_segments_max_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_segments_half_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_pms_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_fmac_bus_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_rx_hrf_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_ilkn_tx_hrf_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_data_rx_hrf_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_tdm_rx_hrf_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_tx_hrf_credits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_nof_rx_hrf_per_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_watermark_high_elk_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_watermark_low_elk_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_watermark_high_data_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_watermark_low_data_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_pad_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_burst_max_range_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_burst_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_burst_short_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_burst_min_range_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_max_nof_ifs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_max_nof_elk_ifs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_port_0_status_intr_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_port_1_status_intr_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_link_stable_wait_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_interleaved_error_drop_single_context_get_f) (
    int unit);


typedef const dnx_data_nif_ilkn_phys_t *(
    *dnx_data_nif_ilkn_phys_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_nif_ilkn_supported_phys_t *(
    *dnx_data_nif_ilkn_supported_phys_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_nif_ilkn_supported_interfaces_t *(
    *dnx_data_nif_ilkn_supported_interfaces_get_f) (
    int unit,
    int index);


typedef const dnx_data_nif_ilkn_ilkn_cores_t *(
    *dnx_data_nif_ilkn_ilkn_cores_get_f) (
    int unit,
    int ilkn_core_id);


typedef const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *(
    *dnx_data_nif_ilkn_ilkn_cores_clup_facing_get_f) (
    int unit,
    int ilkn_core_id);


typedef const dnx_data_nif_ilkn_supported_device_core_t *(
    *dnx_data_nif_ilkn_supported_device_core_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_nif_ilkn_properties_t *(
    *dnx_data_nif_ilkn_properties_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_nif_ilkn_nif_pms_t *(
    *dnx_data_nif_ilkn_nif_pms_get_f) (
    int unit,
    int pm_id);


typedef const dnx_data_nif_ilkn_fabric_pms_t *(
    *dnx_data_nif_ilkn_fabric_pms_get_f) (
    int unit,
    int pm_id);


typedef const dnx_data_nif_ilkn_nif_lanes_map_t *(
    *dnx_data_nif_ilkn_nif_lanes_map_get_f) (
    int unit,
    int ilkn_core,
    int range_id);


typedef const dnx_data_nif_ilkn_fabric_lanes_map_t *(
    *dnx_data_nif_ilkn_fabric_lanes_map_get_f) (
    int unit,
    int ilkn_core,
    int range_id);


typedef const dnx_data_nif_ilkn_start_tx_threshold_table_t *(
    *dnx_data_nif_ilkn_start_tx_threshold_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_ilkn_connectivity_options_t *(
    *dnx_data_nif_ilkn_connectivity_options_get_f) (
    int unit,
    int ilkn_id,
    int connectivity_mode);



typedef struct
{
    
    dnx_data_nif_ilkn_feature_get_f feature_get;
    
    dnx_data_nif_ilkn_ilu_nof_get_f ilu_nof_get;
    
    dnx_data_nif_ilkn_ilkn_unit_nof_get_f ilkn_unit_nof_get;
    
    dnx_data_nif_ilkn_ilkn_unit_if_nof_get_f ilkn_unit_if_nof_get;
    
    dnx_data_nif_ilkn_ilkn_if_nof_get_f ilkn_if_nof_get;
    
    dnx_data_nif_ilkn_fec_units_per_core_nof_get_f fec_units_per_core_nof_get;
    
    dnx_data_nif_ilkn_nof_lanes_per_fec_unit_get_f nof_lanes_per_fec_unit_get;
    
    dnx_data_nif_ilkn_lanes_max_nof_get_f lanes_max_nof_get;
    
    dnx_data_nif_ilkn_lanes_max_nof_using_fec_get_f lanes_max_nof_using_fec_get;
    
    dnx_data_nif_ilkn_lanes_allowed_nof_get_f lanes_allowed_nof_get;
    
    dnx_data_nif_ilkn_ilkn_over_eth_pms_max_get_f ilkn_over_eth_pms_max_get;
    
    dnx_data_nif_ilkn_segments_max_nof_get_f segments_max_nof_get;
    
    dnx_data_nif_ilkn_segments_half_nof_get_f segments_half_nof_get;
    
    dnx_data_nif_ilkn_pms_nof_get_f pms_nof_get;
    
    dnx_data_nif_ilkn_fmac_bus_size_get_f fmac_bus_size_get;
    
    dnx_data_nif_ilkn_ilkn_rx_hrf_nof_get_f ilkn_rx_hrf_nof_get;
    
    dnx_data_nif_ilkn_ilkn_tx_hrf_nof_get_f ilkn_tx_hrf_nof_get;
    
    dnx_data_nif_ilkn_data_rx_hrf_size_get_f data_rx_hrf_size_get;
    
    dnx_data_nif_ilkn_tdm_rx_hrf_size_get_f tdm_rx_hrf_size_get;
    
    dnx_data_nif_ilkn_tx_hrf_credits_get_f tx_hrf_credits_get;
    
    dnx_data_nif_ilkn_nof_rx_hrf_per_port_get_f nof_rx_hrf_per_port_get;
    
    dnx_data_nif_ilkn_watermark_high_elk_get_f watermark_high_elk_get;
    
    dnx_data_nif_ilkn_watermark_low_elk_get_f watermark_low_elk_get;
    
    dnx_data_nif_ilkn_watermark_high_data_get_f watermark_high_data_get;
    
    dnx_data_nif_ilkn_watermark_low_data_get_f watermark_low_data_get;
    
    dnx_data_nif_ilkn_pad_size_get_f pad_size_get;
    
    dnx_data_nif_ilkn_burst_max_range_max_get_f burst_max_range_max_get;
    
    dnx_data_nif_ilkn_burst_min_get_f burst_min_get;
    
    dnx_data_nif_ilkn_burst_short_get_f burst_short_get;
    
    dnx_data_nif_ilkn_burst_min_range_max_get_f burst_min_range_max_get;
    
    dnx_data_nif_ilkn_max_nof_ifs_get_f max_nof_ifs_get;
    
    dnx_data_nif_ilkn_max_nof_elk_ifs_get_f max_nof_elk_ifs_get;
    
    dnx_data_nif_ilkn_port_0_status_intr_id_get_f port_0_status_intr_id_get;
    
    dnx_data_nif_ilkn_port_1_status_intr_id_get_f port_1_status_intr_id_get;
    
    dnx_data_nif_ilkn_link_stable_wait_get_f link_stable_wait_get;
    
    dnx_data_nif_ilkn_interleaved_error_drop_single_context_get_f interleaved_error_drop_single_context_get;
    
    dnx_data_nif_ilkn_phys_get_f phys_get;
    
    dnxc_data_table_info_get_f phys_info_get;
    
    dnx_data_nif_ilkn_supported_phys_get_f supported_phys_get;
    
    dnxc_data_table_info_get_f supported_phys_info_get;
    
    dnx_data_nif_ilkn_supported_interfaces_get_f supported_interfaces_get;
    
    dnxc_data_table_info_get_f supported_interfaces_info_get;
    
    dnx_data_nif_ilkn_ilkn_cores_get_f ilkn_cores_get;
    
    dnxc_data_table_info_get_f ilkn_cores_info_get;
    
    dnx_data_nif_ilkn_ilkn_cores_clup_facing_get_f ilkn_cores_clup_facing_get;
    
    dnxc_data_table_info_get_f ilkn_cores_clup_facing_info_get;
    
    dnx_data_nif_ilkn_supported_device_core_get_f supported_device_core_get;
    
    dnxc_data_table_info_get_f supported_device_core_info_get;
    
    dnx_data_nif_ilkn_properties_get_f properties_get;
    
    dnxc_data_table_info_get_f properties_info_get;
    
    dnx_data_nif_ilkn_nif_pms_get_f nif_pms_get;
    
    dnxc_data_table_info_get_f nif_pms_info_get;
    
    dnx_data_nif_ilkn_fabric_pms_get_f fabric_pms_get;
    
    dnxc_data_table_info_get_f fabric_pms_info_get;
    
    dnx_data_nif_ilkn_nif_lanes_map_get_f nif_lanes_map_get;
    
    dnxc_data_table_info_get_f nif_lanes_map_info_get;
    
    dnx_data_nif_ilkn_fabric_lanes_map_get_f fabric_lanes_map_get;
    
    dnxc_data_table_info_get_f fabric_lanes_map_info_get;
    
    dnx_data_nif_ilkn_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
    
    dnx_data_nif_ilkn_connectivity_options_get_f connectivity_options_get;
    
    dnxc_data_table_info_get_f connectivity_options_info_get;
} dnx_data_if_nif_ilkn_t;






typedef struct
{
    uint32 speed;
    uint32 start_thr;
    uint32 start_thr_for_l1;
} dnx_data_nif_eth_start_tx_threshold_table_t;


typedef struct
{
    bcm_pbmp_t phys;
    int tvco_pll_index;
    int phy_addr;
    int is_master;
    portmod_dispatch_type_t dispatch_type;
    int special_pll_check;
    int flr_support;
    int ext_txpi_support;
    int link_recovery_support;
    int measure_rate_on_down_port_supported;
} dnx_data_nif_eth_pm_properties_t;


typedef struct
{
    int pms[DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE];
    imb_dispatch_type_t type;
    int type_index;
} dnx_data_nif_eth_ethu_properties_t;


typedef struct
{
    int phy_index;
    int ethu_index;
    int pm_index;
} dnx_data_nif_eth_phy_map_t;


typedef struct
{
    int is_supported;
} dnx_data_nif_eth_is_nif_loopback_supported_t;


typedef struct
{
    int is_supported;
} dnx_data_nif_eth_mac_padding_t;


typedef struct
{
    int speed;
} dnx_data_nif_eth_max_speed_t;


typedef struct
{
    int speed;
    int nof_lanes;
    bcm_port_phy_fec_t fec_type;
    portmod_dispatch_type_t dispatch_type;
    int is_valid;
    int ptp_support;
    int autoneg_support;
} dnx_data_nif_eth_supported_interfaces_t;


typedef struct
{
    int core_id;
} dnx_data_nif_eth_ethu_per_core_t;


typedef struct
{
    int nof_ethus;
    int first_ethu;
} dnx_data_nif_eth_nif_cores_ethus_t;


typedef struct
{
    int an_mode;
    int speed;
    int nof_lanes;
    bcm_port_phy_fec_t fec_type;
    bcm_port_medium_t medium;
    bcm_port_phy_channel_t channel;
    int is_valid;
} dnx_data_nif_eth_supported_clu_mgu_an_abilities_t;


typedef struct
{
    int channel;
} dnx_data_nif_eth_ingress_reassembly_context_mapping_t;



typedef enum
{
    dnx_data_nif_eth_nif_scheduler_context_per_rmc_support,
    dnx_data_nif_eth_is_rmc_allocation_needed,
    dnx_data_nif_eth_rmc_flush_support,
    dnx_data_nif_eth_is_rmc_low_priority_needs_configuration,
    dnx_data_nif_eth_is_clu_supported,
    dnx_data_nif_eth_is_rx_port_mode_needs_configuration,
    dnx_data_nif_eth_pcs_lane_map_reconfig_support,
    dnx_data_nif_eth_gear_shifter_exists,
    dnx_data_nif_eth_pm8x50_bw_update,
    dnx_data_nif_eth_pm8x50_clk_repeater_disable,
    dnx_data_nif_eth_portmod_thread_disable,
    dnx_data_nif_eth_legacy_interfaces_support,
    dnx_data_nif_eth_tx_fdbk_port_supported,

    
    _dnx_data_nif_eth_feature_nof
} dnx_data_nif_eth_feature_e;



typedef int(
    *dnx_data_nif_eth_feature_get_f) (
    int unit,
    dnx_data_nif_eth_feature_e feature);


typedef uint32(
    *dnx_data_nif_eth_cdu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_clu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nbu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_mgu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdum_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_cdu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_clu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_nbu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_mgu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_total_nof_ethu_pms_in_device_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_cdu_lanes_in_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_clu_lanes_in_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_cdu_pms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_clu_pms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_nbu_lanes_in_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_mgu_lanes_in_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_nbu_pms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_mgu_pms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_ethu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_clu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_max_ethu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_cdu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_clu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_nbu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_mgu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_max_nof_lanes_in_ethu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_total_nof_nbu_lanes_in_device_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_mac_mode_config_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nbu_mac_mode_config_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_mac_mode_config_lanes_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_mac_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nbu_mac_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_mac_lanes_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_ethu_logical_fifo_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_memory_entries_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_clu_memory_entries_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nbu_memory_entries_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_mgu_memory_entries_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_priority_group_nof_entries_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_priority_groups_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_rmcs_per_priority_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_pad_size_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_pad_size_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_packet_size_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_an_max_nof_abilities_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_phy_map_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_vco_factor_for_53p125_rate_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_vco_factor_for_51p5625_rate_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_is_400G_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_is_800G_supported_get_f) (
    int unit);


typedef const dnx_data_nif_eth_start_tx_threshold_table_t *(
    *dnx_data_nif_eth_start_tx_threshold_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_eth_pm_properties_t *(
    *dnx_data_nif_eth_pm_properties_get_f) (
    int unit,
    int pm_index);


typedef const dnx_data_nif_eth_ethu_properties_t *(
    *dnx_data_nif_eth_ethu_properties_get_f) (
    int unit,
    int ethu_index);


typedef const dnx_data_nif_eth_phy_map_t *(
    *dnx_data_nif_eth_phy_map_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_eth_is_nif_loopback_supported_t *(
    *dnx_data_nif_eth_is_nif_loopback_supported_get_f) (
    int unit,
    int type);


typedef const dnx_data_nif_eth_mac_padding_t *(
    *dnx_data_nif_eth_mac_padding_get_f) (
    int unit,
    int type);


typedef const dnx_data_nif_eth_max_speed_t *(
    *dnx_data_nif_eth_max_speed_get_f) (
    int unit,
    int lane_num);


typedef const dnx_data_nif_eth_supported_interfaces_t *(
    *dnx_data_nif_eth_supported_interfaces_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_eth_ethu_per_core_t *(
    *dnx_data_nif_eth_ethu_per_core_get_f) (
    int unit,
    int ethu_index);


typedef const dnx_data_nif_eth_nif_cores_ethus_t *(
    *dnx_data_nif_eth_nif_cores_ethus_get_f) (
    int unit,
    int core_id);


typedef const dnx_data_nif_eth_supported_clu_mgu_an_abilities_t *(
    *dnx_data_nif_eth_supported_clu_mgu_an_abilities_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_eth_ingress_reassembly_context_mapping_t *(
    *dnx_data_nif_eth_ingress_reassembly_context_mapping_get_f) (
    int unit,
    int scheduler_priority);



typedef struct
{
    
    dnx_data_nif_eth_feature_get_f feature_get;
    
    dnx_data_nif_eth_cdu_nof_get_f cdu_nof_get;
    
    dnx_data_nif_eth_clu_nof_get_f clu_nof_get;
    
    dnx_data_nif_eth_nbu_nof_get_f nbu_nof_get;
    
    dnx_data_nif_eth_mgu_nof_get_f mgu_nof_get;
    
    dnx_data_nif_eth_cdum_nof_get_f cdum_nof_get;
    
    dnx_data_nif_eth_nof_pms_in_cdu_get_f nof_pms_in_cdu_get;
    
    dnx_data_nif_eth_nof_pms_in_clu_get_f nof_pms_in_clu_get;
    
    dnx_data_nif_eth_nof_pms_in_nbu_get_f nof_pms_in_nbu_get;
    
    dnx_data_nif_eth_nof_pms_in_mgu_get_f nof_pms_in_mgu_get;
    
    dnx_data_nif_eth_total_nof_ethu_pms_in_device_get_f total_nof_ethu_pms_in_device_get;
    
    dnx_data_nif_eth_nof_cdu_lanes_in_pm_get_f nof_cdu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_clu_lanes_in_pm_get_f nof_clu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_cdu_pms_get_f nof_cdu_pms_get;
    
    dnx_data_nif_eth_nof_clu_pms_get_f nof_clu_pms_get;
    
    dnx_data_nif_eth_nof_nbu_lanes_in_pm_get_f nof_nbu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_mgu_lanes_in_pm_get_f nof_mgu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_nbu_pms_get_f nof_nbu_pms_get;
    
    dnx_data_nif_eth_nof_mgu_pms_get_f nof_mgu_pms_get;
    
    dnx_data_nif_eth_ethu_nof_get_f ethu_nof_get;
    
    dnx_data_nif_eth_cdu_nof_per_core_get_f cdu_nof_per_core_get;
    
    dnx_data_nif_eth_clu_nof_per_core_get_f clu_nof_per_core_get;
    
    dnx_data_nif_eth_max_ethu_nof_per_core_get_f max_ethu_nof_per_core_get;
    
    dnx_data_nif_eth_nof_lanes_in_cdu_get_f nof_lanes_in_cdu_get;
    
    dnx_data_nif_eth_nof_lanes_in_clu_get_f nof_lanes_in_clu_get;
    
    dnx_data_nif_eth_nof_lanes_in_nbu_get_f nof_lanes_in_nbu_get;
    
    dnx_data_nif_eth_nof_lanes_in_mgu_get_f nof_lanes_in_mgu_get;
    
    dnx_data_nif_eth_max_nof_lanes_in_ethu_get_f max_nof_lanes_in_ethu_get;
    
    dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get_f total_nof_cdu_lanes_in_device_get;
    
    dnx_data_nif_eth_total_nof_nbu_lanes_in_device_get_f total_nof_nbu_lanes_in_device_get;
    
    dnx_data_nif_eth_cdu_mac_mode_config_nof_get_f cdu_mac_mode_config_nof_get;
    
    dnx_data_nif_eth_nbu_mac_mode_config_nof_get_f nbu_mac_mode_config_nof_get;
    
    dnx_data_nif_eth_mac_mode_config_lanes_nof_get_f mac_mode_config_lanes_nof_get;
    
    dnx_data_nif_eth_cdu_mac_nof_get_f cdu_mac_nof_get;
    
    dnx_data_nif_eth_nbu_mac_nof_get_f nbu_mac_nof_get;
    
    dnx_data_nif_eth_mac_lanes_nof_get_f mac_lanes_nof_get;
    
    dnx_data_nif_eth_ethu_logical_fifo_nof_get_f ethu_logical_fifo_nof_get;
    
    dnx_data_nif_eth_cdu_memory_entries_nof_get_f cdu_memory_entries_nof_get;
    
    dnx_data_nif_eth_clu_memory_entries_nof_get_f clu_memory_entries_nof_get;
    
    dnx_data_nif_eth_nbu_memory_entries_nof_get_f nbu_memory_entries_nof_get;
    
    dnx_data_nif_eth_mgu_memory_entries_nof_get_f mgu_memory_entries_nof_get;
    
    dnx_data_nif_eth_priority_group_nof_entries_min_get_f priority_group_nof_entries_min_get;
    
    dnx_data_nif_eth_priority_groups_nof_get_f priority_groups_nof_get;
    
    dnx_data_nif_eth_nof_rmcs_per_priority_group_get_f nof_rmcs_per_priority_group_get;
    
    dnx_data_nif_eth_pad_size_min_get_f pad_size_min_get;
    
    dnx_data_nif_eth_pad_size_max_get_f pad_size_max_get;
    
    dnx_data_nif_eth_packet_size_max_get_f packet_size_max_get;
    
    dnx_data_nif_eth_an_max_nof_abilities_get_f an_max_nof_abilities_get;
    
    dnx_data_nif_eth_phy_map_granularity_get_f phy_map_granularity_get;
    
    dnx_data_nif_eth_vco_factor_for_53p125_rate_get_f vco_factor_for_53p125_rate_get;
    
    dnx_data_nif_eth_vco_factor_for_51p5625_rate_get_f vco_factor_for_51p5625_rate_get;
    
    dnx_data_nif_eth_is_400G_supported_get_f is_400G_supported_get;
    
    dnx_data_nif_eth_is_800G_supported_get_f is_800G_supported_get;
    
    dnx_data_nif_eth_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
    
    dnx_data_nif_eth_pm_properties_get_f pm_properties_get;
    
    dnxc_data_table_info_get_f pm_properties_info_get;
    
    dnx_data_nif_eth_ethu_properties_get_f ethu_properties_get;
    
    dnxc_data_table_info_get_f ethu_properties_info_get;
    
    dnx_data_nif_eth_phy_map_get_f phy_map_get;
    
    dnxc_data_table_info_get_f phy_map_info_get;
    
    dnx_data_nif_eth_is_nif_loopback_supported_get_f is_nif_loopback_supported_get;
    
    dnxc_data_table_info_get_f is_nif_loopback_supported_info_get;
    
    dnx_data_nif_eth_mac_padding_get_f mac_padding_get;
    
    dnxc_data_table_info_get_f mac_padding_info_get;
    
    dnx_data_nif_eth_max_speed_get_f max_speed_get;
    
    dnxc_data_table_info_get_f max_speed_info_get;
    
    dnx_data_nif_eth_supported_interfaces_get_f supported_interfaces_get;
    
    dnxc_data_table_info_get_f supported_interfaces_info_get;
    
    dnx_data_nif_eth_ethu_per_core_get_f ethu_per_core_get;
    
    dnxc_data_table_info_get_f ethu_per_core_info_get;
    
    dnx_data_nif_eth_nif_cores_ethus_get_f nif_cores_ethus_get;
    
    dnxc_data_table_info_get_f nif_cores_ethus_info_get;
    
    dnx_data_nif_eth_supported_clu_mgu_an_abilities_get_f supported_clu_mgu_an_abilities_get;
    
    dnxc_data_table_info_get_f supported_clu_mgu_an_abilities_info_get;
    
    dnx_data_nif_eth_ingress_reassembly_context_mapping_get_f ingress_reassembly_context_mapping_get;
    
    dnxc_data_table_info_get_f ingress_reassembly_context_mapping_info_get;
} dnx_data_if_nif_eth_t;







typedef enum
{

    
    _dnx_data_nif_simulator_feature_nof
} dnx_data_nif_simulator_feature_e;



typedef int(
    *dnx_data_nif_simulator_feature_get_f) (
    int unit,
    dnx_data_nif_simulator_feature_e feature);


typedef uint32(
    *dnx_data_nif_simulator_cdu_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_simulator_clu_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_simulator_nbu_type_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_simulator_mgu_type_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_simulator_feature_get_f feature_get;
    
    dnx_data_nif_simulator_cdu_type_get_f cdu_type_get;
    
    dnx_data_nif_simulator_clu_type_get_f clu_type_get;
    
    dnx_data_nif_simulator_nbu_type_get_f nbu_type_get;
    
    dnx_data_nif_simulator_mgu_type_get_f mgu_type_get;
} dnx_data_if_nif_simulator_t;






typedef struct
{
    int block_id;
    int backup_block_id;
} dnx_data_nif_framer_b66switch_t;


typedef struct
{
    int block_id;
    int backup_block_id;
} dnx_data_nif_framer_mac_info_t;


typedef struct
{
    int valid;
    int is_ptp;
    int block_id;
} dnx_data_nif_framer_mgmt_channel_info_t;



typedef enum
{
    dnx_data_nif_framer_is_supported,
    dnx_data_nif_framer_disable_l1,
    dnx_data_nif_framer_is_clock_independent,
    dnx_data_nif_framer_l1_eth_supported,
    dnx_data_nif_framer_eth_use_flexe_group,
    dnx_data_nif_framer_embeded_mac_in_busa_busc,
    dnx_data_nif_framer_power_down_supported,

    
    _dnx_data_nif_framer_feature_nof
} dnx_data_nif_framer_feature_e;



typedef int(
    *dnx_data_nif_framer_feature_get_f) (
    int unit,
    dnx_data_nif_framer_feature_e feature);


typedef uint32(
    *dnx_data_nif_framer_nof_units_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_b66switch_backup_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_gen_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_nof_mgmt_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_clock_mhz_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_ptp_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_oam_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_nof_mac1_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_nof_mac2_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_framer_nof_pms_per_framer_get_f) (
    int unit);


typedef const dnx_data_nif_framer_b66switch_t *(
    *dnx_data_nif_framer_b66switch_get_f) (
    int unit,
    int port_type);


typedef const dnx_data_nif_framer_mac_info_t *(
    *dnx_data_nif_framer_mac_info_get_f) (
    int unit,
    int index);


typedef const dnx_data_nif_framer_mgmt_channel_info_t *(
    *dnx_data_nif_framer_mgmt_channel_info_get_f) (
    int unit,
    int channel);



typedef struct
{
    
    dnx_data_nif_framer_feature_get_f feature_get;
    
    dnx_data_nif_framer_nof_units_get_f nof_units_get;
    
    dnx_data_nif_framer_b66switch_backup_channels_get_f b66switch_backup_channels_get;
    
    dnx_data_nif_framer_gen_get_f gen_get;
    
    dnx_data_nif_framer_nof_mgmt_channels_get_f nof_mgmt_channels_get;
    
    dnx_data_nif_framer_clock_mhz_get_f clock_mhz_get;
    
    dnx_data_nif_framer_ptp_channel_max_bandwidth_get_f ptp_channel_max_bandwidth_get;
    
    dnx_data_nif_framer_oam_channel_max_bandwidth_get_f oam_channel_max_bandwidth_get;
    
    dnx_data_nif_framer_nof_mac1_channels_get_f nof_mac1_channels_get;
    
    dnx_data_nif_framer_nof_mac2_channels_get_f nof_mac2_channels_get;
    
    dnx_data_nif_framer_nof_pms_per_framer_get_f nof_pms_per_framer_get;
    
    dnx_data_nif_framer_b66switch_get_f b66switch_get;
    
    dnxc_data_table_info_get_f b66switch_info_get;
    
    dnx_data_nif_framer_mac_info_get_f mac_info_get;
    
    dnxc_data_table_info_get_f mac_info_info_get;
    
    dnx_data_nif_framer_mgmt_channel_info_get_f mgmt_channel_info_get;
    
    dnxc_data_table_info_get_f mgmt_channel_info_info_get;
} dnx_data_if_nif_framer_t;






typedef struct
{
    uint32 speed;
    uint32 start_thr;
    uint32 start_thr_for_l1;
} dnx_data_nif_mac_client_start_tx_threshold_table_t;


typedef struct
{
    uint32 eth_speed;
    uint32 client_speed;
    uint32 tx_thr;
} dnx_data_nif_mac_client_l1_mismatch_rate_table_t;


typedef struct
{
    uint32 speed;
    uint32 speed_group;
    uint32 num_credit;
    uint32 tx_threshold;
} dnx_data_nif_mac_client_speed_tx_property_table_t;



typedef enum
{
    dnx_data_nif_mac_client_prd_is_bypassed,
    dnx_data_nif_mac_client_rmc_cal_per_priority,
    dnx_data_nif_mac_client_double_slots_for_all_client,
    dnx_data_nif_mac_client_l1_mismatch_rate_support,
    dnx_data_nif_mac_client_double_slots_for_small_client,
    dnx_data_nif_mac_client_avoid_bw_verification,
    dnx_data_nif_mac_client_disable_l1_mismatch_rate_checking,
    dnx_data_nif_mac_client_is_l1_support,
    dnx_data_nif_mac_client_is_speed_supported,
    dnx_data_nif_mac_client_is_tx_crcmode_supported,
    dnx_data_nif_mac_client_is_tx_pading_supported,
    dnx_data_nif_mac_client_is_pfc_pause_supported,
    dnx_data_nif_mac_client_is_loopback_supported,
    dnx_data_nif_mac_client_sb_tx_fifo_supported,
    dnx_data_nif_mac_client_sb_fifo_size_configurable,
    dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported,
    dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported,
    dnx_data_nif_mac_client_enc_dec_configurable,

    
    _dnx_data_nif_mac_client_feature_nof
} dnx_data_nif_mac_client_feature_e;



typedef int(
    *dnx_data_nif_mac_client_feature_get_f) (
    int unit,
    dnx_data_nif_mac_client_feature_e feature);


typedef uint32(
    *dnx_data_nif_mac_client_nof_normal_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_special_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_channel_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_max_nof_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_logical_fifos_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_oam_client_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_ptp_client_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_oam_tx_cal_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_ptp_tx_cal_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_priority_groups_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_ptp_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_oam_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_mem_entries_per_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_ptp_oam_fifo_entries_in_sb_rx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_average_ipg_for_l1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_mac_tx_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_rmc_cal_nof_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_mac_timeslots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_tx_average_ipg_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_tx_threshold_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_sb_fifo_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_sb_cal_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_sb_fifos_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_sb_cal_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_tinymac_clock_mhz_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_async_fifo_delay_cycles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_pipe_delay_cycles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_mac_client_mac_pad_size_get_f) (
    int unit);


typedef const dnx_data_nif_mac_client_start_tx_threshold_table_t *(
    *dnx_data_nif_mac_client_start_tx_threshold_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_mac_client_l1_mismatch_rate_table_t *(
    *dnx_data_nif_mac_client_l1_mismatch_rate_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_mac_client_speed_tx_property_table_t *(
    *dnx_data_nif_mac_client_speed_tx_property_table_get_f) (
    int unit,
    int idx);



typedef struct
{
    
    dnx_data_nif_mac_client_feature_get_f feature_get;
    
    dnx_data_nif_mac_client_nof_normal_clients_get_f nof_normal_clients_get;
    
    dnx_data_nif_mac_client_nof_special_clients_get_f nof_special_clients_get;
    
    dnx_data_nif_mac_client_nof_clients_get_f nof_clients_get;
    
    dnx_data_nif_mac_client_channel_base_get_f channel_base_get;
    
    dnx_data_nif_mac_client_max_nof_slots_get_f max_nof_slots_get;
    
    dnx_data_nif_mac_client_nof_logical_fifos_get_f nof_logical_fifos_get;
    
    dnx_data_nif_mac_client_oam_client_channel_get_f oam_client_channel_get;
    
    dnx_data_nif_mac_client_ptp_client_channel_get_f ptp_client_channel_get;
    
    dnx_data_nif_mac_client_oam_tx_cal_slot_get_f oam_tx_cal_slot_get;
    
    dnx_data_nif_mac_client_ptp_tx_cal_slot_get_f ptp_tx_cal_slot_get;
    
    dnx_data_nif_mac_client_priority_groups_nof_get_f priority_groups_nof_get;
    
    dnx_data_nif_mac_client_ptp_channel_max_bandwidth_get_f ptp_channel_max_bandwidth_get;
    
    dnx_data_nif_mac_client_oam_channel_max_bandwidth_get_f oam_channel_max_bandwidth_get;
    
    dnx_data_nif_mac_client_nof_mem_entries_per_slot_get_f nof_mem_entries_per_slot_get;
    
    dnx_data_nif_mac_client_ptp_oam_fifo_entries_in_sb_rx_get_f ptp_oam_fifo_entries_in_sb_rx_get;
    
    dnx_data_nif_mac_client_average_ipg_for_l1_get_f average_ipg_for_l1_get;
    
    dnx_data_nif_mac_client_mac_tx_threshold_get_f mac_tx_threshold_get;
    
    dnx_data_nif_mac_client_rmc_cal_nof_slots_get_f rmc_cal_nof_slots_get;
    
    dnx_data_nif_mac_client_nof_mac_timeslots_get_f nof_mac_timeslots_get;
    
    dnx_data_nif_mac_client_tx_average_ipg_max_get_f tx_average_ipg_max_get;
    
    dnx_data_nif_mac_client_tx_threshold_max_get_f tx_threshold_max_get;
    
    dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_get_f tx_fifo_cell_cnt_max_get;
    
    dnx_data_nif_mac_client_sb_fifo_granularity_get_f sb_fifo_granularity_get;
    
    dnx_data_nif_mac_client_sb_cal_granularity_get_f sb_cal_granularity_get;
    
    dnx_data_nif_mac_client_nof_sb_fifos_get_f nof_sb_fifos_get;
    
    dnx_data_nif_mac_client_nof_sb_cal_slots_get_f nof_sb_cal_slots_get;
    
    dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_get_f nof_units_per_sb_tx_fifo_get;
    
    dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_get_f nof_units_per_sb_rx_fifo_get;
    
    dnx_data_nif_mac_client_tinymac_clock_mhz_get_f tinymac_clock_mhz_get;
    
    dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_get_f sb_tx_init_credit_for_5g_get;
    
    dnx_data_nif_mac_client_async_fifo_delay_cycles_get_f async_fifo_delay_cycles_get;
    
    dnx_data_nif_mac_client_pipe_delay_cycles_get_f pipe_delay_cycles_get;
    
    dnx_data_nif_mac_client_mac_pad_size_get_f mac_pad_size_get;
    
    dnx_data_nif_mac_client_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
    
    dnx_data_nif_mac_client_l1_mismatch_rate_table_get_f l1_mismatch_rate_table_get;
    
    dnxc_data_table_info_get_f l1_mismatch_rate_table_info_get;
    
    dnx_data_nif_mac_client_speed_tx_property_table_get_f speed_tx_property_table_get;
    
    dnxc_data_table_info_get_f speed_tx_property_table_info_get;
} dnx_data_if_nif_mac_client_t;






typedef struct
{
    uint32 packet_size;
} dnx_data_nif_sar_client_sar_cell_mode_to_size_t;


typedef struct
{
    int low_level;
    int high_level;
} dnx_data_nif_sar_client_sar_jitter_fifo_levels_t;



typedef enum
{
    dnx_data_nif_sar_client_feu_ilkn_burst_size_support,
    dnx_data_nif_sar_client_connect_to_66bswitch,
    dnx_data_nif_sar_client_is_tdm_port,
    dnx_data_nif_sar_client_per_channel_cell_mode,

    
    _dnx_data_nif_sar_client_feature_nof
} dnx_data_nif_sar_client_feature_e;



typedef int(
    *dnx_data_nif_sar_client_feature_get_f) (
    int unit,
    dnx_data_nif_sar_client_feature_e feature);


typedef uint32(
    *dnx_data_nif_sar_client_nof_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_sar_client_ilkn_burst_size_in_feu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_sar_client_nof_sar_timeslots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_sar_client_channel_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_sar_client_sar_jitter_tolerance_max_level_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_sar_client_cal_granularity_get_f) (
    int unit);


typedef const dnx_data_nif_sar_client_sar_cell_mode_to_size_t *(
    *dnx_data_nif_sar_client_sar_cell_mode_to_size_get_f) (
    int unit,
    int cell_mode);


typedef const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t *(
    *dnx_data_nif_sar_client_sar_jitter_fifo_levels_get_f) (
    int unit,
    int tolerance_level);



typedef struct
{
    
    dnx_data_nif_sar_client_feature_get_f feature_get;
    
    dnx_data_nif_sar_client_nof_clients_get_f nof_clients_get;
    
    dnx_data_nif_sar_client_ilkn_burst_size_in_feu_get_f ilkn_burst_size_in_feu_get;
    
    dnx_data_nif_sar_client_nof_sar_timeslots_get_f nof_sar_timeslots_get;
    
    dnx_data_nif_sar_client_channel_base_get_f channel_base_get;
    
    dnx_data_nif_sar_client_sar_jitter_tolerance_max_level_get_f sar_jitter_tolerance_max_level_get;
    
    dnx_data_nif_sar_client_cal_granularity_get_f cal_granularity_get;
    
    dnx_data_nif_sar_client_sar_cell_mode_to_size_get_f sar_cell_mode_to_size_get;
    
    dnxc_data_table_info_get_f sar_cell_mode_to_size_info_get;
    
    dnx_data_nif_sar_client_sar_jitter_fifo_levels_get_f sar_jitter_fifo_levels_get;
    
    dnxc_data_table_info_get_f sar_jitter_fifo_levels_info_get;
} dnx_data_if_nif_sar_client_t;






typedef struct
{
    int speed;
    int min_logical_phy_id;
    int max_logical_phy_id;
    int flexe_core_port[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS];
    int nof_instances;
    int instance_id[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES];
} dnx_data_nif_flexe_phy_info_t;


typedef struct
{
    int nif_pms[DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE];
} dnx_data_nif_flexe_supported_pms_t;


typedef struct
{
    int val;
} dnx_data_nif_flexe_client_speed_granularity_t;


typedef struct
{
    int val;
} dnx_data_nif_flexe_nb_tx_delay_table_t;



typedef enum
{
    dnx_data_nif_flexe_half_capability,
    dnx_data_nif_flexe_pcs_port_enable_support,
    dnx_data_nif_flexe_rx_valid_in_lf,
    dnx_data_nif_flexe_disable_oam_over_flexe,
    dnx_data_nif_flexe_nb_rx_port_reset_support,
    dnx_data_nif_flexe_toggle_tx_reset_in_nb,
    dnx_data_nif_flexe_pcs_to_flexe_port_map_support,
    dnx_data_nif_flexe_nb_cal_slot_enable_support,
    dnx_data_nif_flexe_nb_cal_switch_enable_support,
    dnx_data_nif_flexe_nb_dynamic_async_fifo,
    dnx_data_nif_flexe_multi_device_mode_support,
    dnx_data_nif_flexe_core_port_need_alloc,
    dnx_data_nif_flexe_multi_flexe_slot_mode_support,
    dnx_data_nif_flexe_unaware_support,
    dnx_data_nif_flexe_nb_tx_cal_support,
    dnx_data_nif_flexe_nb_tx_delay_per_pm,

    
    _dnx_data_nif_flexe_feature_nof
} dnx_data_nif_flexe_feature_e;



typedef int(
    *dnx_data_nif_flexe_feature_get_f) (
    int unit,
    dnx_data_nif_flexe_feature_e feature);


typedef uint32(
    *dnx_data_nif_flexe_feu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_group_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_min_group_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_tx_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_pcs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_phy_speeds_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_phy_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_flexe_core_speed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_flexe_core_clock_source_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_nof_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_min_client_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_client_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_flexe_instances_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_flexe_lphys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_distributed_ilu_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_nif_rate_centralized_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_nif_rate_distributed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_ilu_rate_distributed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_flexe_core_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_cal_nof_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_cal_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_non_5g_slot_bw_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_cal_slot_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_flexe_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_device_slot_mode_get_f) (
    int unit);


typedef const dnx_data_nif_flexe_phy_info_t *(
    *dnx_data_nif_flexe_phy_info_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_flexe_supported_pms_t *(
    *dnx_data_nif_flexe_supported_pms_get_f) (
    int unit,
    int framer_idx);


typedef const dnx_data_nif_flexe_client_speed_granularity_t *(
    *dnx_data_nif_flexe_client_speed_granularity_get_f) (
    int unit,
    int slot_mode);


typedef const dnx_data_nif_flexe_nb_tx_delay_table_t *(
    *dnx_data_nif_flexe_nb_tx_delay_table_get_f) (
    int unit,
    int pm_index);



typedef struct
{
    
    dnx_data_nif_flexe_feature_get_f feature_get;
    
    dnx_data_nif_flexe_feu_nof_get_f feu_nof_get;
    
    dnx_data_nif_flexe_nof_clients_get_f nof_clients_get;
    
    dnx_data_nif_flexe_nof_groups_get_f nof_groups_get;
    
    dnx_data_nif_flexe_max_group_id_get_f max_group_id_get;
    
    dnx_data_nif_flexe_min_group_id_get_f min_group_id_get;
    
    dnx_data_nif_flexe_nb_tx_delay_get_f nb_tx_delay_get;
    
    dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_get_f nb_tx_fifo_th_for_first_read_get;
    
    dnx_data_nif_flexe_nof_pcs_get_f nof_pcs_get;
    
    dnx_data_nif_flexe_nof_phy_speeds_get_f nof_phy_speeds_get;
    
    dnx_data_nif_flexe_phy_speed_granularity_get_f phy_speed_granularity_get;
    
    dnx_data_nif_flexe_max_flexe_core_speed_get_f max_flexe_core_speed_get;
    
    dnx_data_nif_flexe_flexe_core_clock_source_pm_get_f flexe_core_clock_source_pm_get;
    
    dnx_data_nif_flexe_max_nof_slots_get_f max_nof_slots_get;
    
    dnx_data_nif_flexe_min_client_id_get_f min_client_id_get;
    
    dnx_data_nif_flexe_max_client_id_get_f max_client_id_get;
    
    dnx_data_nif_flexe_nof_flexe_instances_get_f nof_flexe_instances_get;
    
    dnx_data_nif_flexe_nof_flexe_lphys_get_f nof_flexe_lphys_get;
    
    dnx_data_nif_flexe_distributed_ilu_id_get_f distributed_ilu_id_get;
    
    dnx_data_nif_flexe_max_nif_rate_centralized_get_f max_nif_rate_centralized_get;
    
    dnx_data_nif_flexe_max_nif_rate_distributed_get_f max_nif_rate_distributed_get;
    
    dnx_data_nif_flexe_max_ilu_rate_distributed_get_f max_ilu_rate_distributed_get;
    
    dnx_data_nif_flexe_nof_flexe_core_ports_get_f nof_flexe_core_ports_get;
    
    dnx_data_nif_flexe_nb_cal_nof_slots_get_f nb_cal_nof_slots_get;
    
    dnx_data_nif_flexe_nb_cal_granularity_get_f nb_cal_granularity_get;
    
    dnx_data_nif_flexe_non_5g_slot_bw_get_f non_5g_slot_bw_get;
    
    dnx_data_nif_flexe_nb_cal_slot_nof_bits_get_f nb_cal_slot_nof_bits_get;
    
    dnx_data_nif_flexe_flexe_mode_get_f flexe_mode_get;
    
    dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get_f nb_tdm_slot_allocation_mode_get;
    
    dnx_data_nif_flexe_device_slot_mode_get_f device_slot_mode_get;
    
    dnx_data_nif_flexe_phy_info_get_f phy_info_get;
    
    dnxc_data_table_info_get_f phy_info_info_get;
    
    dnx_data_nif_flexe_supported_pms_get_f supported_pms_get;
    
    dnxc_data_table_info_get_f supported_pms_info_get;
    
    dnx_data_nif_flexe_client_speed_granularity_get_f client_speed_granularity_get;
    
    dnxc_data_table_info_get_f client_speed_granularity_info_get;
    
    dnx_data_nif_flexe_nb_tx_delay_table_get_f nb_tx_delay_table_get;
    
    dnxc_data_table_info_get_f nb_tx_delay_table_info_get;
} dnx_data_if_nif_flexe_t;







typedef enum
{
    dnx_data_nif_wbc_is_used,

    
    _dnx_data_nif_wbc_feature_nof
} dnx_data_nif_wbc_feature_e;



typedef int(
    *dnx_data_nif_wbc_feature_get_f) (
    int unit,
    dnx_data_nif_wbc_feature_e feature);



typedef struct
{
    
    dnx_data_nif_wbc_feature_get_f feature_get;
} dnx_data_if_nif_wbc_t;






typedef struct
{
    char *ether_type_name;
    uint32 ether_type_size;
} dnx_data_nif_prd_ether_type_t;


typedef struct
{
    uint32 is_supported;
    uint32 nof_profiles;
} dnx_data_nif_prd_port_profile_map_t;



typedef enum
{
    dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration,
    dnx_data_nif_prd_is_mask_parser_tdm_supported,

    
    _dnx_data_nif_prd_feature_nof
} dnx_data_nif_prd_feature_e;



typedef int(
    *dnx_data_nif_prd_feature_get_f) (
    int unit,
    dnx_data_nif_prd_feature_e feature);


typedef uint32(
    *dnx_data_nif_prd_nof_control_frames_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_ether_type_codes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_tcam_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_tcam_key_offset_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_mpls_special_labels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_priorities_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_custom_ether_type_code_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_custom_ether_type_code_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_ether_type_code_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_ether_type_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_rmc_threshold_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_cdu_rmc_threshold_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_hrf_threshold_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_mpls_special_label_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_flex_key_offset_key_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_flex_key_offset_result_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_tpid_max_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_clu_port_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_framer_port_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_ofr_port_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_rmc_fifo_2_threshold_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_ofr_nif_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_prd_nof_prd_counters_get_f) (
    int unit);


typedef const dnx_data_nif_prd_ether_type_t *(
    *dnx_data_nif_prd_ether_type_get_f) (
    int unit,
    int ether_type_code);


typedef const dnx_data_nif_prd_port_profile_map_t *(
    *dnx_data_nif_prd_port_profile_map_get_f) (
    int unit,
    int type);



typedef struct
{
    
    dnx_data_nif_prd_feature_get_f feature_get;
    
    dnx_data_nif_prd_nof_control_frames_get_f nof_control_frames_get;
    
    dnx_data_nif_prd_nof_ether_type_codes_get_f nof_ether_type_codes_get;
    
    dnx_data_nif_prd_nof_tcam_entries_get_f nof_tcam_entries_get;
    
    dnx_data_nif_prd_tcam_key_offset_size_get_f tcam_key_offset_size_get;
    
    dnx_data_nif_prd_nof_mpls_special_labels_get_f nof_mpls_special_labels_get;
    
    dnx_data_nif_prd_nof_priorities_get_f nof_priorities_get;
    
    dnx_data_nif_prd_custom_ether_type_code_min_get_f custom_ether_type_code_min_get;
    
    dnx_data_nif_prd_custom_ether_type_code_max_get_f custom_ether_type_code_max_get;
    
    dnx_data_nif_prd_ether_type_code_max_get_f ether_type_code_max_get;
    
    dnx_data_nif_prd_ether_type_max_get_f ether_type_max_get;
    
    dnx_data_nif_prd_rmc_threshold_max_get_f rmc_threshold_max_get;
    
    dnx_data_nif_prd_cdu_rmc_threshold_max_get_f cdu_rmc_threshold_max_get;
    
    dnx_data_nif_prd_hrf_threshold_max_get_f hrf_threshold_max_get;
    
    dnx_data_nif_prd_mpls_special_label_max_get_f mpls_special_label_max_get;
    
    dnx_data_nif_prd_flex_key_offset_key_max_get_f flex_key_offset_key_max_get;
    
    dnx_data_nif_prd_flex_key_offset_result_max_get_f flex_key_offset_result_max_get;
    
    dnx_data_nif_prd_tpid_max_get_f tpid_max_get;
    
    dnx_data_nif_prd_nof_clu_port_profiles_get_f nof_clu_port_profiles_get;
    
    dnx_data_nif_prd_nof_framer_port_profiles_get_f nof_framer_port_profiles_get;
    
    dnx_data_nif_prd_nof_ofr_port_profiles_get_f nof_ofr_port_profiles_get;
    
    dnx_data_nif_prd_rmc_fifo_2_threshold_resolution_get_f rmc_fifo_2_threshold_resolution_get;
    
    dnx_data_nif_prd_nof_ofr_nif_interfaces_get_f nof_ofr_nif_interfaces_get;
    
    dnx_data_nif_prd_nof_prd_counters_get_f nof_prd_counters_get;
    
    dnx_data_nif_prd_ether_type_get_f ether_type_get;
    
    dnxc_data_table_info_get_f ether_type_info_get;
    
    dnx_data_nif_prd_port_profile_map_get_f port_profile_map_get;
    
    dnxc_data_table_info_get_f port_profile_map_info_get;
} dnx_data_if_nif_prd_t;






typedef struct
{
    portmod_dispatch_type_t type;
    int instances;
} dnx_data_nif_portmod_pm_types_and_interfaces_t;



typedef enum
{
    dnx_data_nif_portmod_not_implemented,

    
    _dnx_data_nif_portmod_feature_nof
} dnx_data_nif_portmod_feature_e;



typedef int(
    *dnx_data_nif_portmod_feature_get_f) (
    int unit,
    dnx_data_nif_portmod_feature_e feature);


typedef uint32(
    *dnx_data_nif_portmod_pm_types_nof_get_f) (
    int unit);


typedef const dnx_data_nif_portmod_pm_types_and_interfaces_t *(
    *dnx_data_nif_portmod_pm_types_and_interfaces_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_nif_portmod_feature_get_f feature_get;
    
    dnx_data_nif_portmod_pm_types_nof_get_f pm_types_nof_get;
    
    dnx_data_nif_portmod_pm_types_and_interfaces_get_f pm_types_and_interfaces_get;
    
    dnxc_data_table_info_get_f pm_types_and_interfaces_info_get;
} dnx_data_if_nif_portmod_t;






typedef struct
{
    int start_slot;
} dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t;



typedef enum
{
    dnx_data_nif_scheduler_decrement_credit_on_read_enable,
    dnx_data_nif_scheduler_dynamic_scheduling,
    dnx_data_nif_scheduler_hardcoded_lanes_and_priority,

    
    _dnx_data_nif_scheduler_feature_nof
} dnx_data_nif_scheduler_feature_e;



typedef int(
    *dnx_data_nif_scheduler_feature_get_f) (
    int unit,
    dnx_data_nif_scheduler_feature_e feature);


typedef uint32(
    *dnx_data_nif_scheduler_rate_per_ethu_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_rate_per_ilu_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_nof_weight_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_nof_rmc_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_dynamic_scheduling_rate_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_rmc_map_table_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_scheduler_rmc_map_table_entry_resolution_get_f) (
    int unit);


typedef const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t *(
    *dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_get_f) (
    int unit,
    int rmc_id);



typedef struct
{
    
    dnx_data_nif_scheduler_feature_get_f feature_get;
    
    dnx_data_nif_scheduler_rate_per_ethu_bit_get_f rate_per_ethu_bit_get;
    
    dnx_data_nif_scheduler_rate_per_ilu_bit_get_f rate_per_ilu_bit_get;
    
    dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get_f rate_per_cdu_rmc_bit_get;
    
    dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get_f rate_per_clu_rmc_bit_get;
    
    dnx_data_nif_scheduler_nof_weight_bits_get_f nof_weight_bits_get;
    
    dnx_data_nif_scheduler_nof_rmc_bits_get_f nof_rmc_bits_get;
    
    dnx_data_nif_scheduler_dynamic_scheduling_rate_resolution_get_f dynamic_scheduling_rate_resolution_get;
    
    dnx_data_nif_scheduler_rmc_map_table_nof_entries_get_f rmc_map_table_nof_entries_get;
    
    dnx_data_nif_scheduler_rmc_map_table_entry_resolution_get_f rmc_map_table_entry_resolution_get;
    
    dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_get_f rx_scheduler_rmc_entries_map_get;
    
    dnxc_data_table_info_get_f rx_scheduler_rmc_entries_map_info_get;
} dnx_data_if_nif_scheduler_t;







typedef enum
{
    dnx_data_nif_dbal_active_mac_configure,

    
    _dnx_data_nif_dbal_feature_nof
} dnx_data_nif_dbal_feature_e;



typedef int(
    *dnx_data_nif_dbal_feature_get_f) (
    int unit,
    dnx_data_nif_dbal_feature_e feature);


typedef uint32(
    *dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_clu_rx_rmc_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nbu_tx_start_threshold_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_mgu_tx_start_threshold_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nbu_tx_threshold_after_ovf_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_mgu_tx_threshold_after_ovf_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nbu_rx_fifo_srstn_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_mgu_rx_fifo_srstn_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_ilu_burst_min_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nbu_lane_fifo_level_and_occupancy_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_mgu_lane_fifo_level_and_occupancy_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_tx_credits_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nof_rx_fifo_counters_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_nof_tx_fifo_counters_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_dbal_feature_get_f feature_get;
    
    dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get_f cdu_rx_rmc_entry_size_get;
    
    dnx_data_nif_dbal_clu_rx_rmc_entry_size_get_f clu_rx_rmc_entry_size_get;
    
    dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get_f cdu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get_f clu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_nbu_tx_start_threshold_entry_size_get_f nbu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_mgu_tx_start_threshold_entry_size_get_f mgu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_nbu_tx_threshold_after_ovf_entry_size_get_f nbu_tx_threshold_after_ovf_entry_size_get;
    
    dnx_data_nif_dbal_mgu_tx_threshold_after_ovf_entry_size_get_f mgu_tx_threshold_after_ovf_entry_size_get;
    
    dnx_data_nif_dbal_nbu_rx_fifo_srstn_entry_size_get_f nbu_rx_fifo_srstn_entry_size_get;
    
    dnx_data_nif_dbal_mgu_rx_fifo_srstn_entry_size_get_f mgu_rx_fifo_srstn_entry_size_get;
    
    dnx_data_nif_dbal_ilu_burst_min_entry_size_get_f ilu_burst_min_entry_size_get;
    
    dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get_f ethu_rx_rmc_counter_entry_size_get;
    
    dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get_f cdu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get_f clu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_nbu_lane_fifo_level_and_occupancy_entry_size_get_f nbu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_mgu_lane_fifo_level_and_occupancy_entry_size_get_f mgu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get_f sch_cnt_dec_threshold_bit_size_get;
    
    dnx_data_nif_dbal_tx_credits_bits_get_f tx_credits_bits_get;
    
    dnx_data_nif_dbal_nof_rx_fifo_counters_get_f nof_rx_fifo_counters_get;
    
    dnx_data_nif_dbal_nof_tx_fifo_counters_get_f nof_tx_fifo_counters_get;
} dnx_data_if_nif_dbal_t;







typedef enum
{
    dnx_data_nif_features_pm_mode_0122_invalid,

    
    _dnx_data_nif_features_feature_nof
} dnx_data_nif_features_feature_e;



typedef int(
    *dnx_data_nif_features_feature_get_f) (
    int unit,
    dnx_data_nif_features_feature_e feature);



typedef struct
{
    
    dnx_data_nif_features_feature_get_f feature_get;
} dnx_data_if_nif_features_t;







typedef enum
{
    dnx_data_nif_arb_is_supported,
    dnx_data_nif_arb_is_used,

    
    _dnx_data_nif_arb_feature_nof
} dnx_data_nif_arb_feature_e;



typedef int(
    *dnx_data_nif_arb_feature_get_f) (
    int unit,
    dnx_data_nif_arb_feature_e feature);


typedef uint32(
    *dnx_data_nif_arb_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_rx_qpms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_rx_ppms_in_qpm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_lanes_in_qpm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_map_destinations_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_max_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_rx_sources_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_sch_calendar_nof_modes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_qpm_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_sch_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_cdpm_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_tmac_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_qpm_calendar_nof_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_sch_calendar_nof_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_qpm_port_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_sch_port_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_tx_eth_port_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_tx_tmac_port_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_tx_tmac_nof_sections_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_entries_in_tmac_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_min_port_speed_for_link_list_section_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_link_list_sections_denominator_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_high_start_tx_speed_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_low_start_tx_speed_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_high_start_tx_speed_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_middle_start_tx_speed_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_low_start_tx_speed_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_max_value_for_tx_start_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_arb_rx_pm_default_min_pkt_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_arb_feature_get_f feature_get;
    
    dnx_data_nif_arb_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_nif_arb_nof_rx_qpms_get_f nof_rx_qpms_get;
    
    dnx_data_nif_arb_nof_rx_ppms_in_qpm_get_f nof_rx_ppms_in_qpm_get;
    
    dnx_data_nif_arb_nof_lanes_in_qpm_get_f nof_lanes_in_qpm_get;
    
    dnx_data_nif_arb_nof_map_destinations_get_f nof_map_destinations_get;
    
    dnx_data_nif_arb_max_calendar_length_get_f max_calendar_length_get;
    
    dnx_data_nif_arb_nof_rx_sources_get_f nof_rx_sources_get;
    
    dnx_data_nif_arb_rx_sch_calendar_nof_modes_get_f rx_sch_calendar_nof_modes_get;
    
    dnx_data_nif_arb_rx_qpm_calendar_length_get_f rx_qpm_calendar_length_get;
    
    dnx_data_nif_arb_rx_sch_calendar_length_get_f rx_sch_calendar_length_get;
    
    dnx_data_nif_arb_cdpm_calendar_length_get_f cdpm_calendar_length_get;
    
    dnx_data_nif_arb_tmac_calendar_length_get_f tmac_calendar_length_get;
    
    dnx_data_nif_arb_rx_qpm_calendar_nof_clients_get_f rx_qpm_calendar_nof_clients_get;
    
    dnx_data_nif_arb_rx_sch_calendar_nof_clients_get_f rx_sch_calendar_nof_clients_get;
    
    dnx_data_nif_arb_rx_qpm_port_speed_granularity_get_f rx_qpm_port_speed_granularity_get;
    
    dnx_data_nif_arb_rx_sch_port_speed_granularity_get_f rx_sch_port_speed_granularity_get;
    
    dnx_data_nif_arb_tx_eth_port_speed_granularity_get_f tx_eth_port_speed_granularity_get;
    
    dnx_data_nif_arb_tx_tmac_port_speed_granularity_get_f tx_tmac_port_speed_granularity_get;
    
    dnx_data_nif_arb_tx_tmac_nof_sections_get_f tx_tmac_nof_sections_get;
    
    dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_get_f tx_tmac_link_list_speed_granularity_get;
    
    dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_get_f nof_entries_in_memory_row_for_rx_calendar_get;
    
    dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_get_f nof_bit_per_entry_in_rx_calendar_get;
    
    dnx_data_nif_arb_nof_entries_in_tmac_calendar_get_f nof_entries_in_tmac_calendar_get;
    
    dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_get_f nof_bit_per_entry_in_tmac_calendar_get;
    
    dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_get_f nof_bit_per_entry_in_ppm_calendar_get;
    
    dnx_data_nif_arb_min_port_speed_for_link_list_section_get_f min_port_speed_for_link_list_section_get;
    
    dnx_data_nif_arb_link_list_sections_denominator_get_f link_list_sections_denominator_get;
    
    dnx_data_nif_arb_high_start_tx_speed_threshold_get_f high_start_tx_speed_threshold_get;
    
    dnx_data_nif_arb_low_start_tx_speed_threshold_get_f low_start_tx_speed_threshold_get;
    
    dnx_data_nif_arb_high_start_tx_speed_value_get_f high_start_tx_speed_value_get;
    
    dnx_data_nif_arb_middle_start_tx_speed_value_get_f middle_start_tx_speed_value_get;
    
    dnx_data_nif_arb_low_start_tx_speed_value_get_f low_start_tx_speed_value_get;
    
    dnx_data_nif_arb_max_value_for_tx_start_threshold_get_f max_value_for_tx_start_threshold_get;
    
    dnx_data_nif_arb_rx_pm_default_min_pkt_size_get_f rx_pm_default_min_pkt_size_get;
} dnx_data_if_nif_arb_t;







typedef enum
{
    dnx_data_nif_ofr_is_supported,
    dnx_data_nif_ofr_is_used,

    
    _dnx_data_nif_ofr_feature_nof
} dnx_data_nif_ofr_feature_e;



typedef int(
    *dnx_data_nif_ofr_feature_get_f) (
    int unit,
    dnx_data_nif_ofr_feature_e feature);


typedef uint32(
    *dnx_data_nif_ofr_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_rmc_per_priority_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_total_rmcs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_rx_mem_sections_per_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_rx_memory_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_rx_memory_sections_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_rx_memory_entries_per_section_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_rx_sch_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ofr_nof_bytes_per_memory_section_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_ofr_feature_get_f feature_get;
    
    dnx_data_nif_ofr_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_nif_ofr_nof_rmc_per_priority_group_get_f nof_rmc_per_priority_group_get;
    
    dnx_data_nif_ofr_nof_total_rmcs_get_f nof_total_rmcs_get;
    
    dnx_data_nif_ofr_nof_rx_mem_sections_per_group_get_f nof_rx_mem_sections_per_group_get;
    
    dnx_data_nif_ofr_nof_rx_memory_groups_get_f nof_rx_memory_groups_get;
    
    dnx_data_nif_ofr_nof_rx_memory_sections_get_f nof_rx_memory_sections_get;
    
    dnx_data_nif_ofr_nof_rx_memory_entries_per_section_get_f nof_rx_memory_entries_per_section_get;
    
    dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_get_f rx_memory_link_list_speed_granularity_get;
    
    dnx_data_nif_ofr_rx_sch_granularity_get_f rx_sch_granularity_get;
    
    dnx_data_nif_ofr_nof_bytes_per_memory_section_get_f nof_bytes_per_memory_section_get;
} dnx_data_if_nif_ofr_t;







typedef enum
{
    dnx_data_nif_oft_is_supported,
    dnx_data_nif_oft_is_used,

    
    _dnx_data_nif_oft_feature_nof
} dnx_data_nif_oft_feature_e;



typedef int(
    *dnx_data_nif_oft_feature_get_f) (
    int unit,
    dnx_data_nif_oft_feature_e feature);


typedef uint32(
    *dnx_data_nif_oft_calendar_length_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_nof_sections_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_nof_internal_calendar_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_port_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_oft_calendar_speed_granularity_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_oft_feature_get_f feature_get;
    
    dnx_data_nif_oft_calendar_length_get_f calendar_length_get;
    
    dnx_data_nif_oft_nof_sections_get_f nof_sections_get;
    
    dnx_data_nif_oft_nof_internal_calendar_entries_get_f nof_internal_calendar_entries_get;
    
    dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_get_f nof_bit_per_internal_entry_in_calendar_get;
    
    dnx_data_nif_oft_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_nif_oft_port_speed_granularity_get_f port_speed_granularity_get;
    
    dnx_data_nif_oft_calendar_speed_granularity_get_f calendar_speed_granularity_get;
} dnx_data_if_nif_oft_t;






typedef struct
{
    uint32 is_valid;
} dnx_data_nif_signal_quality_supported_fecs_t;


typedef struct
{
    uint32 is_valid;
} dnx_data_nif_signal_quality_hw_based_solution_t;



typedef enum
{
    dnx_data_nif_signal_quality_is_supported,
    dnx_data_nif_signal_quality_is_sw_supported,
    dnx_data_nif_signal_quality_is_ser_supported,
    dnx_data_nif_signal_quality_is_ber_supported,

    
    _dnx_data_nif_signal_quality_feature_nof
} dnx_data_nif_signal_quality_feature_e;



typedef int(
    *dnx_data_nif_signal_quality_feature_get_f) (
    int unit,
    dnx_data_nif_signal_quality_feature_e feature);


typedef uint32(
    *dnx_data_nif_signal_quality_is_debug_enabled_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_max_degrade_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_min_degrade_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_max_fail_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_min_fail_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_max_interval_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_signal_quality_min_interval_get_f) (
    int unit);


typedef const dnx_data_nif_signal_quality_supported_fecs_t *(
    *dnx_data_nif_signal_quality_supported_fecs_get_f) (
    int unit,
    int fec_type);


typedef const dnx_data_nif_signal_quality_hw_based_solution_t *(
    *dnx_data_nif_signal_quality_hw_based_solution_get_f) (
    int unit,
    int pm_type);



typedef struct
{
    
    dnx_data_nif_signal_quality_feature_get_f feature_get;
    
    dnx_data_nif_signal_quality_is_debug_enabled_get_f is_debug_enabled_get;
    
    dnx_data_nif_signal_quality_max_degrade_threshold_get_f max_degrade_threshold_get;
    
    dnx_data_nif_signal_quality_min_degrade_threshold_get_f min_degrade_threshold_get;
    
    dnx_data_nif_signal_quality_max_fail_threshold_get_f max_fail_threshold_get;
    
    dnx_data_nif_signal_quality_min_fail_threshold_get_f min_fail_threshold_get;
    
    dnx_data_nif_signal_quality_max_interval_get_f max_interval_get;
    
    dnx_data_nif_signal_quality_min_interval_get_f min_interval_get;
    
    dnx_data_nif_signal_quality_supported_fecs_get_f supported_fecs_get;
    
    dnxc_data_table_info_get_f supported_fecs_info_get;
    
    dnx_data_nif_signal_quality_hw_based_solution_get_f hw_based_solution_get;
    
    dnxc_data_table_info_get_f hw_based_solution_info_get;
} dnx_data_if_nif_signal_quality_t;






typedef struct
{
    uint32 speed;
    portmod_dispatch_type_t pm_dispatch_type;
    bcm_port_phy_fec_t fec_type;
    int nof_lanes;
    char *otn_intf_name;
    uint32 is_valid;
} dnx_data_nif_otn_supported_interfaces_t;



typedef enum
{
    dnx_data_nif_otn_is_supported,

    
    _dnx_data_nif_otn_feature_nof
} dnx_data_nif_otn_feature_e;



typedef int(
    *dnx_data_nif_otn_feature_get_f) (
    int unit,
    dnx_data_nif_otn_feature_e feature);


typedef uint32(
    *dnx_data_nif_otn_nof_units_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_otn_nof_lanes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_otn_nof_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_otn_nof_lanes_per_group_get_f) (
    int unit);


typedef const dnx_data_nif_otn_supported_interfaces_t *(
    *dnx_data_nif_otn_supported_interfaces_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_nif_otn_feature_get_f feature_get;
    
    dnx_data_nif_otn_nof_units_get_f nof_units_get;
    
    dnx_data_nif_otn_nof_lanes_get_f nof_lanes_get;
    
    dnx_data_nif_otn_nof_groups_get_f nof_groups_get;
    
    dnx_data_nif_otn_nof_lanes_per_group_get_f nof_lanes_per_group_get;
    
    dnx_data_nif_otn_supported_interfaces_get_f supported_interfaces_get;
    
    dnxc_data_table_info_get_f supported_interfaces_info_get;
} dnx_data_if_nif_otn_t;







typedef enum
{
    dnx_data_nif_flr_is_advanced_flr_supported,

    
    _dnx_data_nif_flr_feature_nof
} dnx_data_nif_flr_feature_e;



typedef int(
    *dnx_data_nif_flr_feature_get_f) (
    int unit,
    dnx_data_nif_flr_feature_e feature);


typedef uint32(
    *dnx_data_nif_flr_max_tick_unit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flr_max_tick_count_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flr_timer_granularity_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_flr_feature_get_f feature_get;
    
    dnx_data_nif_flr_max_tick_unit_get_f max_tick_unit_get;
    
    dnx_data_nif_flr_max_tick_count_get_f max_tick_count_get;
    
    dnx_data_nif_flr_timer_granularity_get_f timer_granularity_get;
} dnx_data_if_nif_flr_t;





typedef struct
{
    
    dnx_data_if_nif_global_t global;
    
    dnx_data_if_nif_phys_t phys;
    
    dnx_data_if_nif_ilkn_t ilkn;
    
    dnx_data_if_nif_eth_t eth;
    
    dnx_data_if_nif_simulator_t simulator;
    
    dnx_data_if_nif_framer_t framer;
    
    dnx_data_if_nif_mac_client_t mac_client;
    
    dnx_data_if_nif_sar_client_t sar_client;
    
    dnx_data_if_nif_flexe_t flexe;
    
    dnx_data_if_nif_wbc_t wbc;
    
    dnx_data_if_nif_prd_t prd;
    
    dnx_data_if_nif_portmod_t portmod;
    
    dnx_data_if_nif_scheduler_t scheduler;
    
    dnx_data_if_nif_dbal_t dbal;
    
    dnx_data_if_nif_features_t features;
    
    dnx_data_if_nif_arb_t arb;
    
    dnx_data_if_nif_ofr_t ofr;
    
    dnx_data_if_nif_oft_t oft;
    
    dnx_data_if_nif_signal_quality_t signal_quality;
    
    dnx_data_if_nif_otn_t otn;
    
    dnx_data_if_nif_flr_t flr;
} dnx_data_if_nif_t;




extern dnx_data_if_nif_t dnx_data_nif;


#endif 

