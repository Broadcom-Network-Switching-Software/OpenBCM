
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_OAM_H_

#define _DNX_DATA_OAM_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_oam.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_oam_init(
    int unit);







typedef enum
{
    dnx_data_oam_general_oam_not_supported,
    dnx_data_oam_general_oam_event_fifo_support,
    dnx_data_oam_general_low_scaling,
    dnx_data_oam_general_max_oam_offset_support,
    dnx_data_oam_general_oam_hlm_support,
    dnx_data_oam_general_oam_sub_type_twamp_tx_support,
    dnx_data_oam_general_oam_twamp_stateless_mode_support,
    dnx_data_oam_general_oam_dmac_mdl_match_check_control_support,
    dnx_data_oam_general_oam_lmdm_flexible_dmac_support,
    dnx_data_oam_general_oam_lmdm_report_mode_support,
    dnx_data_oam_general_oam_various_ep_memory_type_support,
    dnx_data_oam_general_oam_short_group_support,
    dnx_data_oam_general_oam_twamp_flow_id_ext_enable,
    dnx_data_oam_general_oam_tx_inject_lm_responder_support,
    dnx_data_oam_general_oam_y1711_enable,

    
    _dnx_data_oam_general_feature_nof
} dnx_data_oam_general_feature_e;



typedef int(
    *dnx_data_oam_general_feature_get_f) (
    int unit,
    dnx_data_oam_general_feature_e feature);


typedef uint32(
    *dnx_data_oam_general_oam_nof_non_acc_action_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_acc_action_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_bits_acc_action_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_endpoints_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_oamp_meps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_entries_oam_lif_db_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_bits_counter_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_punt_error_codes_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_priority_map_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_force_interface_for_meps_without_counter_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_my_cfm_mac_by_range_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_pemla_channel_types_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_lm_read_index_field_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_identification_appdb_1_field_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_ntp_freq_control_word_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_ntp_time_freq_control_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_IEEE_1588_time_freq_control_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nse_nco_freq_control_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_tod_config_in_eci_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_tod_config_in_ns_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_tod_config_in_ns_version_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_tod_config_in_ns_nof_bits_in_load_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_tod_init_in_oam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_ns_freq_control_upper_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_local_outlif_msb_bit_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_mpls_tp_or_bfd_enable_map_exists_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_counter_interfaces_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_sub_type_v2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_nof_server_device_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_app_id_init_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_slm_dedicated_profile_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_tcam_key_pkt_data_at_fwd_header_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_oam_trap_action_profile_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_general_bfd_nof_cluster_profile_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_oam_general_feature_get_f feature_get;
    
    dnx_data_oam_general_oam_nof_non_acc_action_profiles_get_f oam_nof_non_acc_action_profiles_get;
    
    dnx_data_oam_general_oam_nof_acc_action_profiles_get_f oam_nof_acc_action_profiles_get;
    
    dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_get_f oam_nof_reserved_egr_acc_action_profiles_get;
    
    dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_get_f oam_egr_acc_action_profile_id_for_inject_mpls_get;
    
    dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_get_f oam_nof_bits_non_acc_action_profiles_get;
    
    dnx_data_oam_general_oam_nof_bits_acc_action_profiles_get_f oam_nof_bits_acc_action_profiles_get;
    
    dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_get_f oam_nof_bits_oam_lif_db_key_base_ingress_get;
    
    dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_get_f oam_nof_bits_oam_lif_db_key_base_egress_get;
    
    dnx_data_oam_general_oam_nof_endpoints_get_f oam_nof_endpoints_get;
    
    dnx_data_oam_general_oam_nof_oamp_meps_get_f oam_nof_oamp_meps_get;
    
    dnx_data_oam_general_oam_nof_groups_get_f oam_nof_groups_get;
    
    dnx_data_oam_general_oam_nof_entries_oam_lif_db_get_f oam_nof_entries_oam_lif_db_get;
    
    dnx_data_oam_general_oam_nof_bits_counter_base_get_f oam_nof_bits_counter_base_get;
    
    dnx_data_oam_general_oam_nof_punt_error_codes_get_f oam_nof_punt_error_codes_get;
    
    dnx_data_oam_general_oam_nof_priority_map_profiles_get_f oam_nof_priority_map_profiles_get;
    
    dnx_data_oam_general_oam_force_interface_for_meps_without_counter_get_f oam_force_interface_for_meps_without_counter_get;
    
    dnx_data_oam_general_oam_my_cfm_mac_by_range_get_f oam_my_cfm_mac_by_range_get;
    
    dnx_data_oam_general_oam_nof_pemla_channel_types_get_f oam_nof_pemla_channel_types_get;
    
    dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_get_f oam_mdb_rmep_db_profile_limitation_get;
    
    dnx_data_oam_general_oam_lm_read_index_field_exists_get_f oam_lm_read_index_field_exists_get;
    
    dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_get_f oam_lm_read_per_sub_enable_field_exists_get;
    
    dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_get_f oam_lm_read_per_sub_disable_field_exists_get;
    
    dnx_data_oam_general_oam_identification_appdb_1_field_exists_get_f oam_identification_appdb_1_field_exists_get;
    
    dnx_data_oam_general_oam_ntp_freq_control_word_get_f oam_ntp_freq_control_word_get;
    
    dnx_data_oam_general_oam_ntp_time_freq_control_get_f oam_ntp_time_freq_control_get;
    
    dnx_data_oam_general_oam_IEEE_1588_time_freq_control_get_f oam_IEEE_1588_time_freq_control_get;
    
    dnx_data_oam_general_oam_nse_nco_freq_control_get_f oam_nse_nco_freq_control_get;
    
    dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_get_f oam_nof_labels_for_sd_on_p_node_get;
    
    dnx_data_oam_general_oam_tod_config_in_eci_get_f oam_tod_config_in_eci_get;
    
    dnx_data_oam_general_oam_tod_config_in_ns_get_f oam_tod_config_in_ns_get;
    
    dnx_data_oam_general_oam_tod_config_in_ns_version_get_f oam_tod_config_in_ns_version_get;
    
    dnx_data_oam_general_oam_tod_config_in_ns_nof_bits_in_load_enable_get_f oam_tod_config_in_ns_nof_bits_in_load_enable_get;
    
    dnx_data_oam_general_tod_init_in_oam_get_f tod_init_in_oam_get;
    
    dnx_data_oam_general_oam_ns_freq_control_upper_get_f oam_ns_freq_control_upper_get;
    
    dnx_data_oam_general_oam_local_outlif_msb_bit_index_get_f oam_local_outlif_msb_bit_index_get;
    
    dnx_data_oam_general_oam_mpls_tp_or_bfd_enable_map_exists_get_f oam_mpls_tp_or_bfd_enable_map_exists_get;
    
    dnx_data_oam_general_oam_nof_counter_interfaces_get_f oam_nof_counter_interfaces_get;
    
    dnx_data_oam_general_oam_sub_type_v2_get_f oam_sub_type_v2_get;
    
    dnx_data_oam_general_oam_nof_server_device_profile_get_f oam_nof_server_device_profile_get;
    
    dnx_data_oam_general_oam_app_id_init_get_f oam_app_id_init_get;
    
    dnx_data_oam_general_oam_slm_dedicated_profile_supported_get_f oam_slm_dedicated_profile_supported_get;
    
    dnx_data_oam_general_oam_tcam_key_pkt_data_at_fwd_header_get_f oam_tcam_key_pkt_data_at_fwd_header_get;
    
    dnx_data_oam_general_oam_trap_action_profile_nof_bits_get_f oam_trap_action_profile_nof_bits_get;
    
    dnx_data_oam_general_bfd_nof_cluster_profile_get_f bfd_nof_cluster_profile_get;
} dnx_data_if_oam_general_t;







typedef enum
{
    dnx_data_oam_oamp_oamp_pe_support,
    dnx_data_oam_oamp_oamp_pe_tx_machine_support,
    dnx_data_oam_oamp_oamp_v2,

    
    _dnx_data_oam_oamp_feature_nof
} dnx_data_oam_oamp_feature_e;



typedef int(
    *dnx_data_oam_oamp_feature_get_f) (
    int unit,
    dnx_data_oam_oamp_feature_e feature);


typedef uint32(
    *dnx_data_oam_oamp_nof_mep_db_entry_banks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_mep_db_entries_per_bank_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_scan_rate_bank_exist_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_mep_db_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_max_nof_endpoint_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_max_nof_mep_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_max_value_of_mep_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_umc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_id_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_id_in_rx_pkt_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_mep_db_endpoint_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_icc_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_local_port_2_system_port_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oam_nof_sa_mac_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oam_nof_tpid_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_pe_default_instruction_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_pe_max_nof_instructions_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_pe_nof_fem_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_sub_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_nof_sub_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_full_entry_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_pe_profile_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_rmep_db_entries_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_rmep_max_self_contained_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_rmep_pair_sys_clocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_sys_clocks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_num_clocks_scan_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_scan_sycles_in_us_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_scan_sycles_in_ms_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_version_limitation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_crc_calculation_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_modified_mep_db_structure_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_local_port_2_system_port_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mdb_nof_access_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_punt_packet_destination_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_core_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_punt_packet_pph_ttl_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_lmm_dmm_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_slm_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_sat_rxi_enable_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mdb_reserved_field_size_calculation_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_event_fifo_slicing_size_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_ccm_piggyback_per_priority_with_crc_supported_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_write_complete_checks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oam_nof_trap_code_tcam_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_rmep_db_access_regs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_dev_type_for_server_tests_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_protection_packet_max_header_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_protection_packet_max_len_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_max_number_of_events_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_nof_generic_memory_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_mep_full_entry_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_mep_db_jr1_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_event_thread_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_oamp_dual_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_up_mep_tod_recycle_port_core_0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_up_mep_tod_recycle_port_core_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_server_destination_msb_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_server_destination_lsb_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_intr_oamp_pending_event_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_oamp_intr_oamp_stat_pending_event_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_oam_oamp_feature_get_f feature_get;
    
    dnx_data_oam_oamp_nof_mep_db_entry_banks_get_f nof_mep_db_entry_banks_get;
    
    dnx_data_oam_oamp_nof_mep_db_entries_per_bank_get_f nof_mep_db_entries_per_bank_get;
    
    dnx_data_oam_oamp_scan_rate_bank_exist_get_f scan_rate_bank_exist_get;
    
    dnx_data_oam_oamp_nof_mep_db_entries_get_f nof_mep_db_entries_get;
    
    dnx_data_oam_oamp_max_nof_endpoint_id_get_f max_nof_endpoint_id_get;
    
    dnx_data_oam_oamp_max_nof_mep_id_get_f max_nof_mep_id_get;
    
    dnx_data_oam_oamp_max_value_of_mep_id_get_f max_value_of_mep_id_get;
    
    dnx_data_oam_oamp_nof_umc_get_f nof_umc_get;
    
    dnx_data_oam_oamp_mep_id_nof_bits_get_f mep_id_nof_bits_get;
    
    dnx_data_oam_oamp_mep_id_in_rx_pkt_nof_bits_get_f mep_id_in_rx_pkt_nof_bits_get;
    
    dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_get_f nof_mep_db_entry_banks_for_endpoints_get;
    
    dnx_data_oam_oamp_nof_mep_db_endpoint_entries_get_f nof_mep_db_endpoint_entries_get;
    
    dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_get_f mep_db_da_is_mc_for_short_entries_support_get;
    
    dnx_data_oam_oamp_nof_icc_profiles_get_f nof_icc_profiles_get;
    
    dnx_data_oam_oamp_local_port_2_system_port_size_get_f local_port_2_system_port_size_get;
    
    dnx_data_oam_oamp_oam_nof_sa_mac_profiles_get_f oam_nof_sa_mac_profiles_get;
    
    dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_get_f oam_nof_da_mac_msb_profiles_get;
    
    dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_get_f oam_nof_da_mac_lsb_profiles_get;
    
    dnx_data_oam_oamp_oam_nof_tpid_profiles_get_f oam_nof_tpid_profiles_get;
    
    dnx_data_oam_oamp_oamp_pe_default_instruction_index_get_f oamp_pe_default_instruction_index_get;
    
    dnx_data_oam_oamp_oamp_pe_max_nof_instructions_get_f oamp_pe_max_nof_instructions_get;
    
    dnx_data_oam_oamp_oamp_pe_nof_fem_bits_get_f oamp_pe_nof_fem_bits_get;
    
    dnx_data_oam_oamp_mep_db_sub_entry_size_get_f mep_db_sub_entry_size_get;
    
    dnx_data_oam_oamp_mep_db_nof_sub_entries_get_f mep_db_nof_sub_entries_get;
    
    dnx_data_oam_oamp_mep_db_full_entry_size_get_f mep_db_full_entry_size_get;
    
    dnx_data_oam_oamp_mep_pe_profile_nof_bits_get_f mep_pe_profile_nof_bits_get;
    
    dnx_data_oam_oamp_rmep_db_entries_nof_bits_get_f rmep_db_entries_nof_bits_get;
    
    dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_get_f rmep_db_rmep_index_nof_bits_get;
    
    dnx_data_oam_oamp_rmep_max_self_contained_get_f rmep_max_self_contained_get;
    
    dnx_data_oam_oamp_rmep_pair_sys_clocks_get_f rmep_pair_sys_clocks_get;
    
    dnx_data_oam_oamp_mep_sys_clocks_get_f mep_sys_clocks_get;
    
    dnx_data_oam_oamp_num_clocks_scan_nof_bits_get_f num_clocks_scan_nof_bits_get;
    
    dnx_data_oam_oamp_oamp_scan_sycles_in_us_get_f oamp_scan_sycles_in_us_get;
    
    dnx_data_oam_oamp_oamp_scan_sycles_in_ms_get_f oamp_scan_sycles_in_ms_get;
    
    dnx_data_oam_oamp_mep_db_version_limitation_get_f mep_db_version_limitation_get;
    
    dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_get_f oamp_mep_db_threshold_version_limitation_get;
    
    dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_get_f rmep_db_non_zero_lifetime_units_limitation_get;
    
    dnx_data_oam_oamp_crc_calculation_supported_get_f crc_calculation_supported_get;
    
    dnx_data_oam_oamp_modified_mep_db_structure_get_f modified_mep_db_structure_get;
    
    dnx_data_oam_oamp_local_port_2_system_port_bits_get_f local_port_2_system_port_bits_get;
    
    dnx_data_oam_oamp_mdb_nof_access_ids_get_f mdb_nof_access_ids_get;
    
    dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_get_f bfd_mpls_short_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_get_f bfd_m_hop_short_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_get_f bfd_pwe_short_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_get_f ccm_eth_offload_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_get_f bfd_1_hop_short_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get_f ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get;
    
    dnx_data_oam_oamp_punt_packet_destination_size_get_f punt_packet_destination_size_get;
    
    dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_get_f punt_packet_itmh_destination_full_encoding_supported_get;
    
    dnx_data_oam_oamp_core_nof_bits_get_f core_nof_bits_get;
    
    dnx_data_oam_oamp_punt_packet_pph_ttl_supported_get_f punt_packet_pph_ttl_supported_get;
    
    dnx_data_oam_oamp_lmm_dmm_supported_get_f lmm_dmm_supported_get;
    
    dnx_data_oam_oamp_slm_supported_get_f slm_supported_get;
    
    dnx_data_oam_oamp_oamp_sat_rxi_enable_support_get_f oamp_sat_rxi_enable_support_get;
    
    dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_get_f mdb_tx_rate_lsb_field_present_get;
    
    dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_get_f nof_jumbo_dm_tlv_entries_get;
    
    dnx_data_oam_oamp_mdb_reserved_field_size_calculation_get_f mdb_reserved_field_size_calculation_get;
    
    dnx_data_oam_oamp_event_fifo_slicing_size_offset_get_f event_fifo_slicing_size_offset_get;
    
    dnx_data_oam_oamp_ccm_piggyback_per_priority_with_crc_supported_get_f ccm_piggyback_per_priority_with_crc_supported_get;
    
    dnx_data_oam_oamp_nof_write_complete_checks_get_f nof_write_complete_checks_get;
    
    dnx_data_oam_oamp_oam_nof_trap_code_tcam_get_f oam_nof_trap_code_tcam_get;
    
    dnx_data_oam_oamp_mep_rmep_db_access_regs_get_f mep_rmep_db_access_regs_get;
    
    dnx_data_oam_oamp_dev_type_for_server_tests_get_f dev_type_for_server_tests_get;
    
    dnx_data_oam_oamp_protection_packet_max_header_size_get_f protection_packet_max_header_size_get;
    
    dnx_data_oam_oamp_protection_packet_max_len_size_get_f protection_packet_max_len_size_get;
    
    dnx_data_oam_oamp_max_number_of_events_get_f max_number_of_events_get;
    
    dnx_data_oam_oamp_nof_generic_memory_entries_get_f nof_generic_memory_entries_get;
    
    dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_get_f oamp_nof_scan_rmep_db_entries_get;
    
    dnx_data_oam_oamp_oamp_mep_full_entry_threshold_get_f oamp_mep_full_entry_threshold_get;
    
    dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_get_f oamp_rmep_full_entry_threshold_get;
    
    dnx_data_oam_oamp_mep_db_jr1_mode_get_f mep_db_jr1_mode_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_get_f oamp_fifo_dma_event_interface_enable_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_get_f oamp_fifo_dma_event_interface_buffer_size_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_get_f oamp_fifo_dma_event_interface_timeout_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_get_f oamp_fifo_dma_event_interface_threshold_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_event_thread_priority_get_f oamp_fifo_dma_event_thread_priority_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_get_f oamp_fifo_dma_report_interface_enable_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_get_f oamp_fifo_dma_report_interface_buffer_size_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_get_f oamp_fifo_dma_report_interface_timeout_get;
    
    dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_get_f oamp_fifo_dma_report_interface_threshold_get;
    
    dnx_data_oam_oamp_oamp_dual_mode_get_f oamp_dual_mode_get;
    
    dnx_data_oam_oamp_up_mep_tod_recycle_port_core_0_get_f up_mep_tod_recycle_port_core_0_get;
    
    dnx_data_oam_oamp_up_mep_tod_recycle_port_core_1_get_f up_mep_tod_recycle_port_core_1_get;
    
    dnx_data_oam_oamp_server_destination_msb_bits_get_f server_destination_msb_bits_get;
    
    dnx_data_oam_oamp_server_destination_lsb_bits_get_f server_destination_lsb_bits_get;
    
    dnx_data_oam_oamp_intr_oamp_pending_event_get_f intr_oamp_pending_event_get;
    
    dnx_data_oam_oamp_intr_oamp_stat_pending_event_get_f intr_oamp_stat_pending_event_get;
} dnx_data_if_oam_oamp_t;







typedef enum
{

    
    _dnx_data_oam_property_feature_nof
} dnx_data_oam_property_feature_e;



typedef int(
    *dnx_data_oam_property_feature_get_f) (
    int unit,
    dnx_data_oam_property_feature_e feature);


typedef uint32(
    *dnx_data_oam_property_oam_injected_over_lsp_cnt_get_f) (
    int unit);


typedef uint32(
    *dnx_data_oam_property_lm_dm_per_priority_enable_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_oam_property_feature_get_f feature_get;
    
    dnx_data_oam_property_oam_injected_over_lsp_cnt_get_f oam_injected_over_lsp_cnt_get;
    
    dnx_data_oam_property_lm_dm_per_priority_enable_get_f lm_dm_per_priority_enable_get;
} dnx_data_if_oam_property_t;







typedef enum
{
    dnx_data_oam_feature_slr_scan_mashine_support,
    dnx_data_oam_feature_slm_j1_measure_support,
    dnx_data_oam_feature_oam_offset_supported,
    dnx_data_oam_feature_oam_use_offset_supported,
    dnx_data_oam_feature_oam_statistics_supported,
    dnx_data_oam_feature_lm_read_index_limitation,
    dnx_data_oam_feature_oamp_scanner_limitation,
    dnx_data_oam_feature_oamp_tx_rate_limitation,
    dnx_data_oam_feature_oamp_protection_rmep_id_limitation,
    dnx_data_oam_feature_oamp_generic_response_supported,
    dnx_data_oam_feature_oamp_txm_bank_limitation,
    dnx_data_oam_feature_oamp_rmep_scale_limitataion,
    dnx_data_oam_feature_up_mep_tod_recycle_enable,
    dnx_data_oam_feature_pcp_based_lm_dm_support,
    dnx_data_oam_feature_ptch_oam_offset_limitation,

    
    _dnx_data_oam_feature_feature_nof
} dnx_data_oam_feature_feature_e;



typedef int(
    *dnx_data_oam_feature_feature_get_f) (
    int unit,
    dnx_data_oam_feature_feature_e feature);



typedef struct
{
    
    dnx_data_oam_feature_feature_get_f feature_get;
} dnx_data_if_oam_feature_t;





typedef struct
{
    
    dnx_data_if_oam_general_t general;
    
    dnx_data_if_oam_oamp_t oamp;
    
    dnx_data_if_oam_property_t property;
    
    dnx_data_if_oam_feature_t feature;
} dnx_data_if_oam_t;




extern dnx_data_if_oam_t dnx_data_oam;


#endif 

