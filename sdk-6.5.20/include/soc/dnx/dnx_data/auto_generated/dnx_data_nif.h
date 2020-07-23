

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_NIF_H_

#define _DNX_DATA_NIF_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
    
    dnx_data_nif_global_reassembler,
    
    dnx_data_nif_global_clock_power_down,
    
    dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down,
    
    dnx_data_nif_global_tx_separate_clock_power_down,
    dnx_data_nif_global_ilkn_credit_loss_handling_mode,

    
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
    *dnx_data_nif_global_max_core_access_per_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_nof_nif_interfaces_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_reassembler_fifo_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_last_port_led_scan_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_global_start_tx_threshold_global_get_f) (
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
    
    dnx_data_nif_global_max_core_access_per_port_get_f max_core_access_per_port_get;
    
    dnx_data_nif_global_nof_nif_interfaces_per_core_get_f nof_nif_interfaces_per_core_get;
    
    dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get_f nif_interface_id_to_unit_id_granularity_get;
    
    dnx_data_nif_global_reassembler_fifo_threshold_get_f reassembler_fifo_threshold_get;
    
    dnx_data_nif_global_last_port_led_scan_get_f last_port_led_scan_get;
    
    dnx_data_nif_global_start_tx_threshold_global_get_f start_tx_threshold_global_get;
    
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



typedef enum
{

    
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
    *dnx_data_nif_phys_nof_phys_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_pm8x50_gen_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_phys_is_pam4_speed_supported_get_f) (
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



typedef struct
{
    
    dnx_data_nif_phys_feature_get_f feature_get;
    
    dnx_data_nif_phys_nof_phys_get_f nof_phys_get;
    
    dnx_data_nif_phys_nof_phys_per_core_get_f nof_phys_per_core_get;
    
    dnx_data_nif_phys_pm8x50_gen_get_f pm8x50_gen_get;
    
    dnx_data_nif_phys_is_pam4_speed_supported_get_f is_pam4_speed_supported_get;
    
    dnx_data_nif_phys_general_get_f general_get;
    
    dnxc_data_table_info_get_f general_info_get;
    
    dnx_data_nif_phys_polarity_get_f polarity_get;
    
    dnxc_data_table_info_get_f polarity_info_get;
    
    dnx_data_nif_phys_core_phys_map_get_f core_phys_map_get;
    
    dnxc_data_table_info_get_f core_phys_map_info_get;
    
    dnx_data_nif_phys_vco_div_get_f vco_div_get;
    
    dnxc_data_table_info_get_f vco_div_info_get;
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
    
    uint32 nif_pm_ids[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    
    uint32 fabric_pm_ids[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
} dnx_data_nif_ilkn_ilkn_pms_t;


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



typedef enum
{
    
    dnx_data_nif_ilkn_is_supported,
    
    dnx_data_nif_ilkn_is_ilu_supported,
    
    dnx_data_nif_ilkn_credits_selector,
    
    dnx_data_nif_ilkn_over_fabric_per_core_selector,
    
    dnx_data_nif_ilkn_cdu_ilkn_selectors,
    
    dnx_data_nif_ilkn_clu_ilkn_selectors,
    
    dnx_data_nif_ilkn_clu_ilkn_direction_selectors,
    
    dnx_data_nif_ilkn_support_burst_interleaving,
    
    dnx_data_nif_ilkn_support_xpmd_if,
    
    dnx_data_nif_ilkn_support_aligner,
    
    dnx_data_nif_ilkn_scheduler_context_per_port_support,
    
    dnx_data_nif_ilkn_support_ilu_burst_max,
    
    dnx_data_nif_ilkn_is_fec_supported,
    
    dnx_data_nif_ilkn_is_fec_bypass_supported,
    
    dnx_data_nif_ilkn_fec_units_in_cdu,
    
    dnx_data_nif_ilkn_hrf_flush_support,
    
    dnx_data_nif_ilkn_elk_require_core_reset_after_link_training,
    
    dnx_data_nif_ilkn_reset_core_after_link_training,
    
    dnx_data_nif_ilkn_ilkn_over_fabric_only,
    
    dnx_data_nif_ilkn_12lanes_mode,

    
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
    *dnx_data_nif_ilkn_is_20G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_25G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_27G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_28G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_53G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_nif_56G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_is_fabric_56G_speed_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_ilkn_pad_size_get_f) (
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


typedef const dnx_data_nif_ilkn_ilkn_pms_t *(
    *dnx_data_nif_ilkn_ilkn_pms_get_f) (
    int unit,
    int ilkn_id);


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
    
    dnx_data_nif_ilkn_is_20G_speed_supported_get_f is_20G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_25G_speed_supported_get_f is_25G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_27G_speed_supported_get_f is_27G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_28G_speed_supported_get_f is_28G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_53G_speed_supported_get_f is_53G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_nif_56G_speed_supported_get_f is_nif_56G_speed_supported_get;
    
    dnx_data_nif_ilkn_is_fabric_56G_speed_supported_get_f is_fabric_56G_speed_supported_get;
    
    dnx_data_nif_ilkn_pad_size_get_f pad_size_get;
    
    dnx_data_nif_ilkn_max_nof_ifs_get_f max_nof_ifs_get;
    
    dnx_data_nif_ilkn_max_nof_elk_ifs_get_f max_nof_elk_ifs_get;
    
    dnx_data_nif_ilkn_port_0_status_intr_id_get_f port_0_status_intr_id_get;
    
    dnx_data_nif_ilkn_port_1_status_intr_id_get_f port_1_status_intr_id_get;
    
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
    
    dnx_data_nif_ilkn_ilkn_pms_get_f ilkn_pms_get;
    
    dnxc_data_table_info_get_f ilkn_pms_info_get;
    
    dnx_data_nif_ilkn_nif_lanes_map_get_f nif_lanes_map_get;
    
    dnxc_data_table_info_get_f nif_lanes_map_info_get;
    
    dnx_data_nif_ilkn_fabric_lanes_map_get_f fabric_lanes_map_get;
    
    dnxc_data_table_info_get_f fabric_lanes_map_info_get;
    
    dnx_data_nif_ilkn_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
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
    
    int speed;
} dnx_data_nif_eth_max_speed_t;


typedef struct
{
    
    int speed;
    
    int nof_lanes;
    
    bcm_port_phy_fec_t fec_type;
    
    portmod_dispatch_type_t dispatch_type;
    
    int is_valid;
} dnx_data_nif_eth_supported_interfaces_t;


typedef struct
{
    
    int an_mode;
    
    int speed;
    
    int nof_lanes;
    
    bcm_port_phy_fec_t fec_type;
    
    bcm_port_medium_t medium;
    
    bcm_port_phy_channel_t channel;
    
    int is_valid;
} dnx_data_nif_eth_supported_an_abilities_t;



typedef enum
{
    
    dnx_data_nif_eth_nif_scheduler_context_per_rmc_support,
    
    dnx_data_nif_eth_rmc_flush_support,
    
    dnx_data_nif_eth_is_rmc_low_priority_needs_configuration,
    
    dnx_data_nif_eth_is_clu_supported,
    
    dnx_data_nif_eth_is_rx_port_mode_needs_configuration,
    
    dnx_data_nif_eth_pcs_lane_map_reconfig_support,
    
    dnx_data_nif_eth_gear_shifter_exists,
    
    dnx_data_nif_eth_pm8x50_bw_update,
    
    dnx_data_nif_eth_portmod_thread_disable,

    
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
    *dnx_data_nif_eth_cdum_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_cdu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_pms_in_clu_get_f) (
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
    *dnx_data_nif_eth_ethu_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_ethu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_clu_nof_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_cdu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_nof_lanes_in_clu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_max_nof_lanes_in_ethu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_mac_mode_config_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_mac_mode_config_lanes_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_cdu_mac_nof_get_f) (
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
    *dnx_data_nif_eth_priority_group_nof_entries_min_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_eth_priority_groups_nof_get_f) (
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
    *dnx_data_nif_eth_is_400G_supported_get_f) (
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


typedef const dnx_data_nif_eth_max_speed_t *(
    *dnx_data_nif_eth_max_speed_get_f) (
    int unit,
    int lane_num);


typedef const dnx_data_nif_eth_supported_interfaces_t *(
    *dnx_data_nif_eth_supported_interfaces_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_eth_supported_an_abilities_t *(
    *dnx_data_nif_eth_supported_an_abilities_get_f) (
    int unit,
    int idx);



typedef struct
{
    
    dnx_data_nif_eth_feature_get_f feature_get;
    
    dnx_data_nif_eth_cdu_nof_get_f cdu_nof_get;
    
    dnx_data_nif_eth_clu_nof_get_f clu_nof_get;
    
    dnx_data_nif_eth_cdum_nof_get_f cdum_nof_get;
    
    dnx_data_nif_eth_nof_pms_in_cdu_get_f nof_pms_in_cdu_get;
    
    dnx_data_nif_eth_nof_pms_in_clu_get_f nof_pms_in_clu_get;
    
    dnx_data_nif_eth_total_nof_ethu_pms_in_device_get_f total_nof_ethu_pms_in_device_get;
    
    dnx_data_nif_eth_nof_cdu_lanes_in_pm_get_f nof_cdu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_clu_lanes_in_pm_get_f nof_clu_lanes_in_pm_get;
    
    dnx_data_nif_eth_nof_cdu_pms_get_f nof_cdu_pms_get;
    
    dnx_data_nif_eth_nof_clu_pms_get_f nof_clu_pms_get;
    
    dnx_data_nif_eth_ethu_nof_get_f ethu_nof_get;
    
    dnx_data_nif_eth_ethu_nof_per_core_get_f ethu_nof_per_core_get;
    
    dnx_data_nif_eth_cdu_nof_per_core_get_f cdu_nof_per_core_get;
    
    dnx_data_nif_eth_clu_nof_per_core_get_f clu_nof_per_core_get;
    
    dnx_data_nif_eth_nof_lanes_in_cdu_get_f nof_lanes_in_cdu_get;
    
    dnx_data_nif_eth_nof_lanes_in_clu_get_f nof_lanes_in_clu_get;
    
    dnx_data_nif_eth_max_nof_lanes_in_ethu_get_f max_nof_lanes_in_ethu_get;
    
    dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get_f total_nof_cdu_lanes_in_device_get;
    
    dnx_data_nif_eth_cdu_mac_mode_config_nof_get_f cdu_mac_mode_config_nof_get;
    
    dnx_data_nif_eth_mac_mode_config_lanes_nof_get_f mac_mode_config_lanes_nof_get;
    
    dnx_data_nif_eth_cdu_mac_nof_get_f cdu_mac_nof_get;
    
    dnx_data_nif_eth_mac_lanes_nof_get_f mac_lanes_nof_get;
    
    dnx_data_nif_eth_ethu_logical_fifo_nof_get_f ethu_logical_fifo_nof_get;
    
    dnx_data_nif_eth_cdu_memory_entries_nof_get_f cdu_memory_entries_nof_get;
    
    dnx_data_nif_eth_clu_memory_entries_nof_get_f clu_memory_entries_nof_get;
    
    dnx_data_nif_eth_priority_group_nof_entries_min_get_f priority_group_nof_entries_min_get;
    
    dnx_data_nif_eth_priority_groups_nof_get_f priority_groups_nof_get;
    
    dnx_data_nif_eth_pad_size_min_get_f pad_size_min_get;
    
    dnx_data_nif_eth_pad_size_max_get_f pad_size_max_get;
    
    dnx_data_nif_eth_packet_size_max_get_f packet_size_max_get;
    
    dnx_data_nif_eth_an_max_nof_abilities_get_f an_max_nof_abilities_get;
    
    dnx_data_nif_eth_phy_map_granularity_get_f phy_map_granularity_get;
    
    dnx_data_nif_eth_is_400G_supported_get_f is_400G_supported_get;
    
    dnx_data_nif_eth_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
    
    dnx_data_nif_eth_pm_properties_get_f pm_properties_get;
    
    dnxc_data_table_info_get_f pm_properties_info_get;
    
    dnx_data_nif_eth_ethu_properties_get_f ethu_properties_get;
    
    dnxc_data_table_info_get_f ethu_properties_info_get;
    
    dnx_data_nif_eth_phy_map_get_f phy_map_get;
    
    dnxc_data_table_info_get_f phy_map_info_get;
    
    dnx_data_nif_eth_max_speed_get_f max_speed_get;
    
    dnxc_data_table_info_get_f max_speed_info_get;
    
    dnx_data_nif_eth_supported_interfaces_get_f supported_interfaces_get;
    
    dnxc_data_table_info_get_f supported_interfaces_info_get;
    
    dnx_data_nif_eth_supported_an_abilities_get_f supported_an_abilities_get;
    
    dnxc_data_table_info_get_f supported_an_abilities_info_get;
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



typedef struct
{
    
    dnx_data_nif_simulator_feature_get_f feature_get;
    
    dnx_data_nif_simulator_cdu_type_get_f cdu_type_get;
    
    dnx_data_nif_simulator_clu_type_get_f clu_type_get;
} dnx_data_if_nif_simulator_t;






typedef struct
{
    
    int speed;
    
    int min_logical_phy_id;
    
    int max_logical_phy_id;
    
    int flexe_core_port[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS];
} dnx_data_nif_flexe_phy_info_t;


typedef struct
{
    
    uint32 speed;
    
    uint32 start_thr;
    
    uint32 start_thr_for_l1;
} dnx_data_nif_flexe_start_tx_threshold_table_t;


typedef struct
{
    
    int nif_pms[DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE];
} dnx_data_nif_flexe_supported_pms_t;


typedef struct
{
    
    uint32 eth_speed;
    
    uint32 client_speed;
    
    uint32 tx_thr;
} dnx_data_nif_flexe_l1_mismatch_rate_table_t;


typedef struct
{
    
    uint32 packet_size;
} dnx_data_nif_flexe_sar_cell_mode_to_size_t;



typedef enum
{
    
    dnx_data_nif_flexe_is_supported,
    
    dnx_data_nif_flexe_is_led_supported,
    
    dnx_data_nif_flexe_prd_is_bypassed,
    
    dnx_data_nif_flexe_double_slots_for_small_client,
    
    dnx_data_nif_flexe_double_slots_for_all_client,
    
    dnx_data_nif_flexe_half_capability,
    
    dnx_data_nif_flexe_rmc_cal_per_priority,
    
    dnx_data_nif_flexe_avoid_bw_verification,
    
    dnx_data_nif_flexe_pcs_port_enable_support,
    
    dnx_data_nif_flexe_rx_valid_in_lf,
    
    dnx_data_nif_flexe_l1_mismatch_rate_support,
    
    dnx_data_nif_flexe_disable_l1_mismatch_rate_checking,
    
    dnx_data_nif_flexe_feu_ilkn_burst_size_support,
    
    dnx_data_nif_flexe_disable_l1,
    
    dnx_data_nif_flexe_disable_oam_over_flexe,

    
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
    *dnx_data_nif_flexe_nof_flexe_clients_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_special_clients_get_f) (
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
    *dnx_data_nif_flexe_nof_logical_fifos_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_sar_channel_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_mac_channel_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_client_speed_granularity_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_sb_rx_fifos_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_oam_client_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_ptp_client_channel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_oam_tx_cal_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_ptp_tx_cal_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_priority_groups_nof_get_f) (
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
    *dnx_data_nif_flexe_ptp_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_oam_channel_max_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_mem_entries_per_slot_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_ptp_oam_fifo_entries_in_sb_rx_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_flexe_instances_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_flexe_lphys_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_sar_timeslots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_mac_timeslots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_distributed_ilu_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_nif_rate_centralized_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_rmc_cal_nof_slots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_nif_rate_distributed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_max_ilu_rate_distributed_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_ilkn_burst_size_in_feu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_average_ipg_for_l1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_mac_tx_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_flexe_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nof_pms_per_feu_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get_f) (
    int unit);


typedef const dnx_data_nif_flexe_phy_info_t *(
    *dnx_data_nif_flexe_phy_info_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_flexe_start_tx_threshold_table_t *(
    *dnx_data_nif_flexe_start_tx_threshold_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_flexe_supported_pms_t *(
    *dnx_data_nif_flexe_supported_pms_get_f) (
    int unit,
    int feu_idx);


typedef const dnx_data_nif_flexe_l1_mismatch_rate_table_t *(
    *dnx_data_nif_flexe_l1_mismatch_rate_table_get_f) (
    int unit,
    int idx);


typedef const dnx_data_nif_flexe_sar_cell_mode_to_size_t *(
    *dnx_data_nif_flexe_sar_cell_mode_to_size_get_f) (
    int unit,
    int cell_mode);



typedef struct
{
    
    dnx_data_nif_flexe_feature_get_f feature_get;
    
    dnx_data_nif_flexe_feu_nof_get_f feu_nof_get;
    
    dnx_data_nif_flexe_nof_clients_get_f nof_clients_get;
    
    dnx_data_nif_flexe_nof_flexe_clients_get_f nof_flexe_clients_get;
    
    dnx_data_nif_flexe_nof_special_clients_get_f nof_special_clients_get;
    
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
    
    dnx_data_nif_flexe_nof_logical_fifos_get_f nof_logical_fifos_get;
    
    dnx_data_nif_flexe_sar_channel_base_get_f sar_channel_base_get;
    
    dnx_data_nif_flexe_mac_channel_base_get_f mac_channel_base_get;
    
    dnx_data_nif_flexe_client_speed_granularity_get_f client_speed_granularity_get;
    
    dnx_data_nif_flexe_nof_sb_rx_fifos_get_f nof_sb_rx_fifos_get;
    
    dnx_data_nif_flexe_oam_client_channel_get_f oam_client_channel_get;
    
    dnx_data_nif_flexe_ptp_client_channel_get_f ptp_client_channel_get;
    
    dnx_data_nif_flexe_oam_tx_cal_slot_get_f oam_tx_cal_slot_get;
    
    dnx_data_nif_flexe_ptp_tx_cal_slot_get_f ptp_tx_cal_slot_get;
    
    dnx_data_nif_flexe_priority_groups_nof_get_f priority_groups_nof_get;
    
    dnx_data_nif_flexe_max_nof_slots_get_f max_nof_slots_get;
    
    dnx_data_nif_flexe_min_client_id_get_f min_client_id_get;
    
    dnx_data_nif_flexe_max_client_id_get_f max_client_id_get;
    
    dnx_data_nif_flexe_ptp_channel_max_bandwidth_get_f ptp_channel_max_bandwidth_get;
    
    dnx_data_nif_flexe_oam_channel_max_bandwidth_get_f oam_channel_max_bandwidth_get;
    
    dnx_data_nif_flexe_nof_mem_entries_per_slot_get_f nof_mem_entries_per_slot_get;
    
    dnx_data_nif_flexe_ptp_oam_fifo_entries_in_sb_rx_get_f ptp_oam_fifo_entries_in_sb_rx_get;
    
    dnx_data_nif_flexe_nof_flexe_instances_get_f nof_flexe_instances_get;
    
    dnx_data_nif_flexe_nof_flexe_lphys_get_f nof_flexe_lphys_get;
    
    dnx_data_nif_flexe_nof_sar_timeslots_get_f nof_sar_timeslots_get;
    
    dnx_data_nif_flexe_nof_mac_timeslots_get_f nof_mac_timeslots_get;
    
    dnx_data_nif_flexe_distributed_ilu_id_get_f distributed_ilu_id_get;
    
    dnx_data_nif_flexe_max_nif_rate_centralized_get_f max_nif_rate_centralized_get;
    
    dnx_data_nif_flexe_rmc_cal_nof_slots_get_f rmc_cal_nof_slots_get;
    
    dnx_data_nif_flexe_max_nif_rate_distributed_get_f max_nif_rate_distributed_get;
    
    dnx_data_nif_flexe_max_ilu_rate_distributed_get_f max_ilu_rate_distributed_get;
    
    dnx_data_nif_flexe_ilkn_burst_size_in_feu_get_f ilkn_burst_size_in_feu_get;
    
    dnx_data_nif_flexe_average_ipg_for_l1_get_f average_ipg_for_l1_get;
    
    dnx_data_nif_flexe_mac_tx_threshold_get_f mac_tx_threshold_get;
    
    dnx_data_nif_flexe_flexe_mode_get_f flexe_mode_get;
    
    dnx_data_nif_flexe_nof_pms_per_feu_get_f nof_pms_per_feu_get;
    
    dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get_f nb_tdm_slot_allocation_mode_get;
    
    dnx_data_nif_flexe_phy_info_get_f phy_info_get;
    
    dnxc_data_table_info_get_f phy_info_info_get;
    
    dnx_data_nif_flexe_start_tx_threshold_table_get_f start_tx_threshold_table_get;
    
    dnxc_data_table_info_get_f start_tx_threshold_table_info_get;
    
    dnx_data_nif_flexe_supported_pms_get_f supported_pms_get;
    
    dnxc_data_table_info_get_f supported_pms_info_get;
    
    dnx_data_nif_flexe_l1_mismatch_rate_table_get_f l1_mismatch_rate_table_get;
    
    dnxc_data_table_info_get_f l1_mismatch_rate_table_info_get;
    
    dnx_data_nif_flexe_sar_cell_mode_to_size_get_f sar_cell_mode_to_size_get;
    
    dnxc_data_table_info_get_f sar_cell_mode_to_size_info_get;
} dnx_data_if_nif_flexe_t;






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
    *dnx_data_nif_prd_nof_feu_port_profiles_get_f) (
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
    
    dnx_data_nif_prd_nof_feu_port_profiles_get_f nof_feu_port_profiles_get;
    
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







typedef enum
{
    
    dnx_data_nif_scheduler_decrement_credit_on_read_enable,

    
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



typedef struct
{
    
    dnx_data_nif_scheduler_feature_get_f feature_get;
    
    dnx_data_nif_scheduler_rate_per_ethu_bit_get_f rate_per_ethu_bit_get;
    
    dnx_data_nif_scheduler_rate_per_ilu_bit_get_f rate_per_ilu_bit_get;
    
    dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get_f rate_per_cdu_rmc_bit_get;
    
    dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get_f rate_per_clu_rmc_bit_get;
    
    dnx_data_nif_scheduler_nof_weight_bits_get_f nof_weight_bits_get;
    
    dnx_data_nif_scheduler_nof_rmc_bits_get_f nof_rmc_bits_get;
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
    *dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_nif_dbal_tx_credits_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_nif_dbal_feature_get_f feature_get;
    
    dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get_f cdu_rx_rmc_entry_size_get;
    
    dnx_data_nif_dbal_clu_rx_rmc_entry_size_get_f clu_rx_rmc_entry_size_get;
    
    dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get_f cdu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get_f clu_tx_start_threshold_entry_size_get;
    
    dnx_data_nif_dbal_ilu_burst_min_entry_size_get_f ilu_burst_min_entry_size_get;
    
    dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get_f ethu_rx_rmc_counter_entry_size_get;
    
    dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get_f cdu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get_f clu_lane_fifo_level_and_occupancy_entry_size_get;
    
    dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get_f sch_cnt_dec_threshold_bit_size_get;
    
    dnx_data_nif_dbal_tx_credits_bits_get_f tx_credits_bits_get;
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





typedef struct
{
    
    dnx_data_if_nif_global_t global;
    
    dnx_data_if_nif_phys_t phys;
    
    dnx_data_if_nif_ilkn_t ilkn;
    
    dnx_data_if_nif_eth_t eth;
    
    dnx_data_if_nif_simulator_t simulator;
    
    dnx_data_if_nif_flexe_t flexe;
    
    dnx_data_if_nif_prd_t prd;
    
    dnx_data_if_nif_portmod_t portmod;
    
    dnx_data_if_nif_scheduler_t scheduler;
    
    dnx_data_if_nif_dbal_t dbal;
    
    dnx_data_if_nif_features_t features;
} dnx_data_if_nif_t;




extern dnx_data_if_nif_t dnx_data_nif;


#endif 

