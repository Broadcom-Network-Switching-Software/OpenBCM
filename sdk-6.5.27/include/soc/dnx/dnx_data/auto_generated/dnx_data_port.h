
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PORT_H_

#define _DNX_DATA_PORT_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_port_init(
    int unit);






typedef struct
{
    bcm_port_if_t interface;
    int nof_lanes;
    int interface_offset;
    bcm_core_t core;
    int tm_port;
    int out_tm_port;
    int channel;
    int is_stif;
    int is_stif_data;
    int is_mc;
    int is_kbp;
    int is_cross_connect;
    int is_flexe_phy;
    int is_flexe_unaware;
    int is_ingress_interleave;
    int is_egress_interleave;
    int is_tdm;
    int is_if_tdm_only;
    int is_if_tdm_hybrid;
    int is_framer_mgmt;
    int is_feedback;
    int sch_priority_propagation_en;
    int frame_preemptable;
    int num_priorities;
    int base_q_pair;
    int num_sch_priorities;
    int base_hr;
    int framer_mgmt_channel;
} dnx_data_port_static_add_ucode_port_t;


typedef struct
{
    int val;
} dnx_data_port_static_add_speed_t;


typedef struct
{
    int speed;
    int nof_lanes;
} dnx_data_port_static_add_ext_stat_speed_t;


typedef struct
{
    int pad_size;
} dnx_data_port_static_add_eth_padding_t;


typedef struct
{
    int val;
} dnx_data_port_static_add_link_training_t;


typedef struct
{
    int val;
} dnx_data_port_static_add_ext_stat_link_training_t;


typedef struct
{
    bcm_port_phy_fec_t val;
} dnx_data_port_static_add_fec_type_t;


typedef struct
{
    soc_dnxc_port_dfe_mode_t dfe;
    int media_type;
    int unreliable_los;
    int cl72_auto_polarity_enable;
    int cl72_restart_timeout_enable;
    soc_dnxc_port_channel_mode_t channel_mode;
} dnx_data_port_static_add_serdes_lane_config_t;


typedef struct
{
    int pre;
    int main;
    int post;
    int pre2;
    int post2;
    int pre3;
    int post3;
    bcm_port_phy_tx_tap_mode_t tx_tap_mode;
    bcm_port_phy_signalling_mode_t signalling_mode;
} dnx_data_port_static_add_serdes_tx_taps_t;


typedef struct
{
    int pre;
    int main;
    int post;
    int pre2;
    int post2;
    int post3;
    int txfir_tap_enable;
} dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t;


typedef struct
{
    int pre;
    int main;
    int post;
    int pre2;
    int post2;
    int post3;
    int txfir_tap_enable;
} dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t;


typedef struct
{
    uint32 val;
} dnx_data_port_static_add_tx_pam4_precoder_t;


typedef struct
{
    uint32 val;
} dnx_data_port_static_add_lp_tx_precoder_t;


typedef struct
{
    int quad_enable;
} dnx_data_port_static_add_fabric_quad_info_t;


typedef struct
{
    int header_type_in;
    int header_type_out;
    int header_type;
} dnx_data_port_static_add_header_type_t;


typedef struct
{
    int exist;
} dnx_data_port_static_add_erp_exist_t;


typedef struct
{
    int val;
} dnx_data_port_static_add_base_flexe_instance_t;


typedef struct
{
    int enable;
    int master;
} dnx_data_port_static_add_cable_swap_info_t;


typedef struct
{
    int flr_fw_support;
    bcm_port_flr_timeout_t timeout;
} dnx_data_port_static_add_flr_config_t;


typedef struct
{
    int timestamp_enable;
    int one_step_enable;
} dnx_data_port_static_add_timestamp_phy_init_t;


typedef struct
{
    int val;
} dnx_data_port_static_add_txpi_mode_t;



typedef enum
{
    dnx_data_port_static_add_base_flexe_instance_exists,

    
    _dnx_data_port_static_add_feature_nof
} dnx_data_port_static_add_feature_e;



typedef int(
    *dnx_data_port_static_add_feature_get_f) (
    int unit,
    dnx_data_port_static_add_feature_e feature);


typedef uint32(
    *dnx_data_port_static_add_default_speed_for_special_if_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_fabric_fw_load_method_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_fabric_fw_crc_check_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_fabric_fw_load_verify_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_nif_fw_load_method_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_nif_fw_crc_check_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_static_add_nif_fw_load_verify_get_f) (
    int unit);


typedef const dnx_data_port_static_add_ucode_port_t *(
    *dnx_data_port_static_add_ucode_port_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_speed_t *(
    *dnx_data_port_static_add_speed_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_ext_stat_speed_t *(
    *dnx_data_port_static_add_ext_stat_speed_get_f) (
    int unit,
    int ext_stat_port);


typedef const dnx_data_port_static_add_eth_padding_t *(
    *dnx_data_port_static_add_eth_padding_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_link_training_t *(
    *dnx_data_port_static_add_link_training_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_ext_stat_link_training_t *(
    *dnx_data_port_static_add_ext_stat_link_training_get_f) (
    int unit,
    int ext_stat_port);


typedef const dnx_data_port_static_add_fec_type_t *(
    *dnx_data_port_static_add_fec_type_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_serdes_lane_config_t *(
    *dnx_data_port_static_add_serdes_lane_config_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_serdes_tx_taps_t *(
    *dnx_data_port_static_add_serdes_tx_taps_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *(
    *dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_get_f) (
    int unit);


typedef const dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *(
    *dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_get_f) (
    int unit,
    int lane);


typedef const dnx_data_port_static_add_tx_pam4_precoder_t *(
    *dnx_data_port_static_add_tx_pam4_precoder_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_lp_tx_precoder_t *(
    *dnx_data_port_static_add_lp_tx_precoder_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_fabric_quad_info_t *(
    *dnx_data_port_static_add_fabric_quad_info_get_f) (
    int unit,
    int quad);


typedef const dnx_data_port_static_add_header_type_t *(
    *dnx_data_port_static_add_header_type_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_erp_exist_t *(
    *dnx_data_port_static_add_erp_exist_get_f) (
    int unit,
    int core);


typedef const dnx_data_port_static_add_base_flexe_instance_t *(
    *dnx_data_port_static_add_base_flexe_instance_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_cable_swap_info_t *(
    *dnx_data_port_static_add_cable_swap_info_get_f) (
    int unit,
    int pm);


typedef const dnx_data_port_static_add_flr_config_t *(
    *dnx_data_port_static_add_flr_config_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_timestamp_phy_init_t *(
    *dnx_data_port_static_add_timestamp_phy_init_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_static_add_txpi_mode_t *(
    *dnx_data_port_static_add_txpi_mode_get_f) (
    int unit,
    int port);



typedef struct
{
    
    dnx_data_port_static_add_feature_get_f feature_get;
    
    dnx_data_port_static_add_default_speed_for_special_if_get_f default_speed_for_special_if_get;
    
    dnx_data_port_static_add_fabric_fw_load_method_get_f fabric_fw_load_method_get;
    
    dnx_data_port_static_add_fabric_fw_crc_check_get_f fabric_fw_crc_check_get;
    
    dnx_data_port_static_add_fabric_fw_load_verify_get_f fabric_fw_load_verify_get;
    
    dnx_data_port_static_add_nif_fw_load_method_get_f nif_fw_load_method_get;
    
    dnx_data_port_static_add_nif_fw_crc_check_get_f nif_fw_crc_check_get;
    
    dnx_data_port_static_add_nif_fw_load_verify_get_f nif_fw_load_verify_get;
    
    dnx_data_port_static_add_ucode_port_get_f ucode_port_get;
    
    dnxc_data_table_info_get_f ucode_port_info_get;
    
    dnx_data_port_static_add_speed_get_f speed_get;
    
    dnxc_data_table_info_get_f speed_info_get;
    
    dnx_data_port_static_add_ext_stat_speed_get_f ext_stat_speed_get;
    
    dnxc_data_table_info_get_f ext_stat_speed_info_get;
    
    dnx_data_port_static_add_eth_padding_get_f eth_padding_get;
    
    dnxc_data_table_info_get_f eth_padding_info_get;
    
    dnx_data_port_static_add_link_training_get_f link_training_get;
    
    dnxc_data_table_info_get_f link_training_info_get;
    
    dnx_data_port_static_add_ext_stat_link_training_get_f ext_stat_link_training_get;
    
    dnxc_data_table_info_get_f ext_stat_link_training_info_get;
    
    dnx_data_port_static_add_fec_type_get_f fec_type_get;
    
    dnxc_data_table_info_get_f fec_type_info_get;
    
    dnx_data_port_static_add_serdes_lane_config_get_f serdes_lane_config_get;
    
    dnxc_data_table_info_get_f serdes_lane_config_info_get;
    
    dnx_data_port_static_add_serdes_tx_taps_get_f serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f serdes_tx_taps_info_get;
    
    dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_get_f ext_stat_global_serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f ext_stat_global_serdes_tx_taps_info_get;
    
    dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_get_f ext_stat_lane_serdes_tx_taps_get;
    
    dnxc_data_table_info_get_f ext_stat_lane_serdes_tx_taps_info_get;
    
    dnx_data_port_static_add_tx_pam4_precoder_get_f tx_pam4_precoder_get;
    
    dnxc_data_table_info_get_f tx_pam4_precoder_info_get;
    
    dnx_data_port_static_add_lp_tx_precoder_get_f lp_tx_precoder_get;
    
    dnxc_data_table_info_get_f lp_tx_precoder_info_get;
    
    dnx_data_port_static_add_fabric_quad_info_get_f fabric_quad_info_get;
    
    dnxc_data_table_info_get_f fabric_quad_info_info_get;
    
    dnx_data_port_static_add_header_type_get_f header_type_get;
    
    dnxc_data_table_info_get_f header_type_info_get;
    
    dnx_data_port_static_add_erp_exist_get_f erp_exist_get;
    
    dnxc_data_table_info_get_f erp_exist_info_get;
    
    dnx_data_port_static_add_base_flexe_instance_get_f base_flexe_instance_get;
    
    dnxc_data_table_info_get_f base_flexe_instance_info_get;
    
    dnx_data_port_static_add_cable_swap_info_get_f cable_swap_info_get;
    
    dnxc_data_table_info_get_f cable_swap_info_info_get;
    
    dnx_data_port_static_add_flr_config_get_f flr_config_get;
    
    dnxc_data_table_info_get_f flr_config_info_get;
    
    dnx_data_port_static_add_timestamp_phy_init_get_f timestamp_phy_init_get;
    
    dnxc_data_table_info_get_f timestamp_phy_init_info_get;
    
    dnx_data_port_static_add_txpi_mode_get_f txpi_mode_get;
    
    dnxc_data_table_info_get_f txpi_mode_info_get;
} dnx_data_if_port_static_add_t;






typedef struct
{
    int first_header_size;
} dnx_data_port_general_first_header_size_t;


typedef struct
{
    uint32 counted_bits;
    uint32 total_bits;
    uint32 max_nof_correctable_symbols;
    uint32 is_valid;
} dnx_data_port_general_fec_compensation_info_t;


typedef struct
{
    int is_serdes_tx_pre3_supported;
    int is_scrambling_disable_supported;
    int is_phy_rx_taps_supported;
} dnx_data_port_general_pm_info_t;


typedef struct
{
    uint8 port_disable_required;
} dnx_data_port_general_phy_controls_features_t;



typedef enum
{
    dnx_data_port_general_unrestricted_portmod_pll,
    dnx_data_port_general_use_portmod_speed_when_not_enabled,
    dnx_data_port_general_is_cable_swap_supported,
    dnx_data_port_general_is_header_type_rch_srv6_usp_psp_supported,
    dnx_data_port_general_is_rch_header_offset_supported,
    dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported,
    dnx_data_port_general_is_fabric_signal_quality_supported,
    dnx_data_port_general_is_port_extender_supported,
    dnx_data_port_general_is_mpls_raw_supported,

    
    _dnx_data_port_general_feature_nof
} dnx_data_port_general_feature_e;



typedef int(
    *dnx_data_port_general_feature_get_f) (
    int unit,
    dnx_data_port_general_feature_e feature);


typedef uint32(
    *dnx_data_port_general_erp_pp_dsp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_nof_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_nof_mirror_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_fabric_phys_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_pp_dsp_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_ingress_vlan_domain_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_vlan_domain_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_ffc_instruction_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_ffc_instruction_offset_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_ffc_instruction_width_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_ffc_instruction_input_offset_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_first_header_size_to_skip_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_first_header_size_to_skip_before_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_first_header_size_to_skip_after_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_prt_ffc_width_start_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_prt_ffc_start_input_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_phys_per_pm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_core_access_per_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_pp_dsp_per_fap_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_ptc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_in_tm_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_out_tm_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_pp_dsp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_pp_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_reserved_pp_dsp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_vlan_membership_if_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_max_vlan_membership_if_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_fabric_port_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_pp_port_bits_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_port_bits_in_pp_bus_get_f) (
    int unit);


typedef uint32(
    *dnx_data_port_general_nof_bits_egress_vlan_membership_if_get_f) (
    int unit);


typedef const dnx_data_port_general_first_header_size_t *(
    *dnx_data_port_general_first_header_size_get_f) (
    int unit,
    int port);


typedef const dnx_data_port_general_fec_compensation_info_t *(
    *dnx_data_port_general_fec_compensation_info_get_f) (
    int unit,
    int fec_type);


typedef const dnx_data_port_general_pm_info_t *(
    *dnx_data_port_general_pm_info_get_f) (
    int unit,
    int pm_type);


typedef const dnx_data_port_general_phy_controls_features_t *(
    *dnx_data_port_general_phy_controls_features_get_f) (
    int unit,
    int phy_control_type);



typedef struct
{
    
    dnx_data_port_general_feature_get_f feature_get;
    
    dnx_data_port_general_erp_pp_dsp_get_f erp_pp_dsp_get;
    
    dnx_data_port_general_max_nof_channels_get_f max_nof_channels_get;
    
    dnx_data_port_general_max_nof_mirror_channels_get_f max_nof_mirror_channels_get;
    
    dnx_data_port_general_fabric_phys_offset_get_f fabric_phys_offset_get;
    
    dnx_data_port_general_pp_dsp_size_get_f pp_dsp_size_get;
    
    dnx_data_port_general_ingress_vlan_domain_size_get_f ingress_vlan_domain_size_get;
    
    dnx_data_port_general_vlan_domain_size_get_f vlan_domain_size_get;
    
    dnx_data_port_general_ffc_instruction_size_get_f ffc_instruction_size_get;
    
    dnx_data_port_general_ffc_instruction_offset_size_get_f ffc_instruction_offset_size_get;
    
    dnx_data_port_general_ffc_instruction_width_size_get_f ffc_instruction_width_size_get;
    
    dnx_data_port_general_ffc_instruction_input_offset_size_get_f ffc_instruction_input_offset_size_get;
    
    dnx_data_port_general_max_first_header_size_to_skip_get_f max_first_header_size_to_skip_get;
    
    dnx_data_port_general_max_first_header_size_to_skip_before_get_f max_first_header_size_to_skip_before_get;
    
    dnx_data_port_general_max_first_header_size_to_skip_after_get_f max_first_header_size_to_skip_after_get;
    
    dnx_data_port_general_prt_ffc_width_start_offset_get_f prt_ffc_width_start_offset_get;
    
    dnx_data_port_general_prt_ffc_start_input_offset_get_f prt_ffc_start_input_offset_get;
    
    dnx_data_port_general_max_phys_per_pm_get_f max_phys_per_pm_get;
    
    dnx_data_port_general_max_core_access_per_port_get_f max_core_access_per_port_get;
    
    dnx_data_port_general_nof_pp_dsp_per_fap_id_get_f nof_pp_dsp_per_fap_id_get;
    
    dnx_data_port_general_nof_ptc_get_f nof_ptc_get;
    
    dnx_data_port_general_nof_in_tm_ports_get_f nof_in_tm_ports_get;
    
    dnx_data_port_general_nof_out_tm_ports_get_f nof_out_tm_ports_get;
    
    dnx_data_port_general_nof_pp_dsp_get_f nof_pp_dsp_get;
    
    dnx_data_port_general_nof_pp_ports_get_f nof_pp_ports_get;
    
    dnx_data_port_general_reserved_pp_dsp_get_f reserved_pp_dsp_get;
    
    dnx_data_port_general_nof_vlan_membership_if_get_f nof_vlan_membership_if_get;
    
    dnx_data_port_general_max_vlan_membership_if_get_f max_vlan_membership_if_get;
    
    dnx_data_port_general_fabric_port_base_get_f fabric_port_base_get;
    
    dnx_data_port_general_pp_port_bits_size_get_f pp_port_bits_size_get;
    
    dnx_data_port_general_nof_port_bits_in_pp_bus_get_f nof_port_bits_in_pp_bus_get;
    
    dnx_data_port_general_nof_bits_egress_vlan_membership_if_get_f nof_bits_egress_vlan_membership_if_get;
    
    dnx_data_port_general_first_header_size_get_f first_header_size_get;
    
    dnxc_data_table_info_get_f first_header_size_info_get;
    
    dnx_data_port_general_fec_compensation_info_get_f fec_compensation_info_get;
    
    dnxc_data_table_info_get_f fec_compensation_info_info_get;
    
    dnx_data_port_general_pm_info_get_f pm_info_get;
    
    dnxc_data_table_info_get_f pm_info_info_get;
    
    dnx_data_port_general_phy_controls_features_get_f phy_controls_features_get;
    
    dnxc_data_table_info_get_f phy_controls_features_info_get;
} dnx_data_if_port_general_t;






typedef struct
{
    portmod_dispatch_type_t pm_type;
    uint32 nof_pms;
    uint32 nof_lanes;
    uint32 nof_lanes_in_pm;
    uint32 nof_memory_entries;
    uint32 rate_per_rmc_bit;
    uint32 precoder_supported;
    dnx_algo_port_mib_counter_set_type_e mib_type;
} dnx_data_port_imb_imb_type_info_t;



typedef enum
{

    
    _dnx_data_port_imb_feature_nof
} dnx_data_port_imb_feature_e;



typedef int(
    *dnx_data_port_imb_feature_get_f) (
    int unit,
    dnx_data_port_imb_feature_e feature);


typedef const dnx_data_port_imb_imb_type_info_t *(
    *dnx_data_port_imb_imb_type_info_get_f) (
    int unit,
    int type);



typedef struct
{
    
    dnx_data_port_imb_feature_get_f feature_get;
    
    dnx_data_port_imb_imb_type_info_get_f imb_type_info_get;
    
    dnxc_data_table_info_get_f imb_type_info_info_get;
} dnx_data_if_port_imb_t;





typedef struct
{
    
    dnx_data_if_port_static_add_t static_add;
    
    dnx_data_if_port_general_t general;
    
    dnx_data_if_port_imb_t imb;
} dnx_data_if_port_t;




extern dnx_data_if_port_t dnx_data_port;


#endif 

