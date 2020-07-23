

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_OAM_H_

#define _DNX_DATA_INTERNAL_OAM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_oam_submodule_general,
    dnx_data_oam_submodule_oamp,
    dnx_data_oam_submodule_property,
    dnx_data_oam_submodule_feature,

    
    _dnx_data_oam_submodule_nof
} dnx_data_oam_submodule_e;








int dnx_data_oam_general_feature_get(
    int unit,
    dnx_data_oam_general_feature_e feature);



typedef enum
{
    dnx_data_oam_general_define_oam_nof_non_acc_action_profiles,
    dnx_data_oam_general_define_oam_nof_acc_action_profiles,
    dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles,
    dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls,
    dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles,
    dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles,
    dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress,
    dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress,
    dnx_data_oam_general_define_oam_nof_endpoints,
    dnx_data_oam_general_define_oam_nof_oamp_meps,
    dnx_data_oam_general_define_oam_nof_groups,
    dnx_data_oam_general_define_oam_nof_entries_oam_lif_db,
    dnx_data_oam_general_define_oam_nof_bits_counter_base,
    dnx_data_oam_general_define_oam_nof_punt_error_codes,
    dnx_data_oam_general_define_oam_nof_priority_map_profiles,
    dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter,
    dnx_data_oam_general_define_oam_my_cfm_mac_by_range,
    dnx_data_oam_general_define_oam_nof_pemla_channel_types,
    dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation,
    dnx_data_oam_general_define_oam_lm_read_index_field_exists,
    dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists,
    dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists,
    dnx_data_oam_general_define_oam_identification_appdb_1_field_exists,
    dnx_data_oam_general_define_oam_ntp_freq_control_word,
    dnx_data_oam_general_define_oam_ntp_time_freq_control,
    dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control,
    dnx_data_oam_general_define_oam_nse_nco_freq_control,
    dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node,
    dnx_data_oam_general_define_oam_tod_config_in_eci,

    
    _dnx_data_oam_general_define_nof
} dnx_data_oam_general_define_e;



uint32 dnx_data_oam_general_oam_nof_non_acc_action_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_acc_action_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_bits_acc_action_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_endpoints_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_oamp_meps_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_groups_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_entries_oam_lif_db_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_bits_counter_base_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_punt_error_codes_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_priority_map_profiles_get(
    int unit);


uint32 dnx_data_oam_general_oam_force_interface_for_meps_without_counter_get(
    int unit);


uint32 dnx_data_oam_general_oam_my_cfm_mac_by_range_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_pemla_channel_types_get(
    int unit);


uint32 dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_get(
    int unit);


uint32 dnx_data_oam_general_oam_lm_read_index_field_exists_get(
    int unit);


uint32 dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_get(
    int unit);


uint32 dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_get(
    int unit);


uint32 dnx_data_oam_general_oam_identification_appdb_1_field_exists_get(
    int unit);


uint32 dnx_data_oam_general_oam_ntp_freq_control_word_get(
    int unit);


uint32 dnx_data_oam_general_oam_ntp_time_freq_control_get(
    int unit);


uint32 dnx_data_oam_general_oam_IEEE_1588_time_freq_control_get(
    int unit);


uint32 dnx_data_oam_general_oam_nse_nco_freq_control_get(
    int unit);


uint32 dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_get(
    int unit);


uint32 dnx_data_oam_general_oam_tod_config_in_eci_get(
    int unit);



typedef enum
{

    
    _dnx_data_oam_general_table_nof
} dnx_data_oam_general_table_e;









int dnx_data_oam_oamp_feature_get(
    int unit,
    dnx_data_oam_oamp_feature_e feature);



typedef enum
{
    dnx_data_oam_oamp_define_nof_mep_db_entry_banks,
    dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank,
    dnx_data_oam_oamp_define_scan_rate_bank_exist,
    dnx_data_oam_oamp_define_nof_mep_db_entries,
    dnx_data_oam_oamp_define_max_nof_endpoint_id,
    dnx_data_oam_oamp_define_max_nof_mep_id,
    dnx_data_oam_oamp_define_max_value_of_mep_id,
    dnx_data_oam_oamp_define_nof_umc,
    dnx_data_oam_oamp_define_mep_id_nof_bits,
    dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints,
    dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries,
    dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support,
    dnx_data_oam_oamp_define_nof_icc_profiles,
    dnx_data_oam_oamp_define_local_port_2_system_port_size,
    dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles,
    dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles,
    dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles,
    dnx_data_oam_oamp_define_oam_nof_tpid_profiles,
    dnx_data_oam_oamp_define_oamp_pe_default_instruction_index,
    dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions,
    dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits,
    dnx_data_oam_oamp_define_mep_db_sub_entry_size,
    dnx_data_oam_oamp_define_mep_db_nof_sub_entries,
    dnx_data_oam_oamp_define_mep_db_full_entry_size,
    dnx_data_oam_oamp_define_mep_pe_profile_nof_bits,
    dnx_data_oam_oamp_define_rmep_db_entries_nof_bits,
    dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits,
    dnx_data_oam_oamp_define_rmep_max_self_contained,
    dnx_data_oam_oamp_define_rmep_sys_clocks,
    dnx_data_oam_oamp_define_mep_sys_clocks,
    dnx_data_oam_oamp_define_mep_db_version_limitation,
    dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation,
    dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation,
    dnx_data_oam_oamp_define_crc_calculation_supported,
    dnx_data_oam_oamp_define_modified_mep_db_structure,
    dnx_data_oam_oamp_define_local_port_2_system_port_bits,
    dnx_data_oam_oamp_define_mdb_nof_access_ids,
    dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits,
    dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits,
    dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits,
    dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits,
    dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits,
    dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits,
    dnx_data_oam_oamp_define_punt_packet_destination_size,
    dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported,
    dnx_data_oam_oamp_define_core_nof_bits,
    dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported,
    dnx_data_oam_oamp_define_lmm_dmm_supported,
    dnx_data_oam_oamp_define_intr_oamp_pending_event,
    dnx_data_oam_oamp_define_intr_oamp_stat_pending_event,
    dnx_data_oam_oamp_define_slm_supported,
    dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present,
    dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries,
    dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation,
    dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries,
    dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold,
    dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold,
    dnx_data_oam_oamp_define_mep_db_jr1_mode,
    dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable,
    dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size,
    dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout,
    dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold,
    dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable,
    dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size,
    dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout,
    dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold,
    dnx_data_oam_oamp_define_server_destination_msb_bits,
    dnx_data_oam_oamp_define_server_destination_lsb_bits,

    
    _dnx_data_oam_oamp_define_nof
} dnx_data_oam_oamp_define_e;



uint32 dnx_data_oam_oamp_nof_mep_db_entry_banks_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_mep_db_entries_per_bank_get(
    int unit);


uint32 dnx_data_oam_oamp_scan_rate_bank_exist_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_mep_db_entries_get(
    int unit);


uint32 dnx_data_oam_oamp_max_nof_endpoint_id_get(
    int unit);


uint32 dnx_data_oam_oamp_max_nof_mep_id_get(
    int unit);


uint32 dnx_data_oam_oamp_max_value_of_mep_id_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_umc_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_id_nof_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_mep_db_endpoint_entries_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_icc_profiles_get(
    int unit);


uint32 dnx_data_oam_oamp_local_port_2_system_port_size_get(
    int unit);


uint32 dnx_data_oam_oamp_oam_nof_sa_mac_profiles_get(
    int unit);


uint32 dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_get(
    int unit);


uint32 dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_get(
    int unit);


uint32 dnx_data_oam_oamp_oam_nof_tpid_profiles_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_pe_default_instruction_index_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_pe_max_nof_instructions_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_pe_nof_fem_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_sub_entry_size_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_nof_sub_entries_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_full_entry_size_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_pe_profile_nof_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_rmep_db_entries_nof_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_rmep_max_self_contained_get(
    int unit);


uint32 dnx_data_oam_oamp_rmep_sys_clocks_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_sys_clocks_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_version_limitation_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_get(
    int unit);


uint32 dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_get(
    int unit);


uint32 dnx_data_oam_oamp_crc_calculation_supported_get(
    int unit);


uint32 dnx_data_oam_oamp_modified_mep_db_structure_get(
    int unit);


uint32 dnx_data_oam_oamp_local_port_2_system_port_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_mdb_nof_access_ids_get(
    int unit);


uint32 dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_punt_packet_destination_size_get(
    int unit);


uint32 dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_get(
    int unit);


uint32 dnx_data_oam_oamp_core_nof_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_punt_packet_pph_ttl_supported_get(
    int unit);


uint32 dnx_data_oam_oamp_lmm_dmm_supported_get(
    int unit);


uint32 dnx_data_oam_oamp_intr_oamp_pending_event_get(
    int unit);


uint32 dnx_data_oam_oamp_intr_oamp_stat_pending_event_get(
    int unit);


uint32 dnx_data_oam_oamp_slm_supported_get(
    int unit);


uint32 dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_get(
    int unit);


uint32 dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_get(
    int unit);


uint32 dnx_data_oam_oamp_mdb_reserved_field_size_calculation_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_mep_full_entry_threshold_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_get(
    int unit);


uint32 dnx_data_oam_oamp_mep_db_jr1_mode_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_get(
    int unit);


uint32 dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_get(
    int unit);


uint32 dnx_data_oam_oamp_server_destination_msb_bits_get(
    int unit);


uint32 dnx_data_oam_oamp_server_destination_lsb_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_oam_oamp_table_nof
} dnx_data_oam_oamp_table_e;









int dnx_data_oam_property_feature_get(
    int unit,
    dnx_data_oam_property_feature_e feature);



typedef enum
{
    dnx_data_oam_property_define_oam_injected_over_lsp_cnt,

    
    _dnx_data_oam_property_define_nof
} dnx_data_oam_property_define_e;



uint32 dnx_data_oam_property_oam_injected_over_lsp_cnt_get(
    int unit);



typedef enum
{

    
    _dnx_data_oam_property_table_nof
} dnx_data_oam_property_table_e;









int dnx_data_oam_feature_feature_get(
    int unit,
    dnx_data_oam_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_oam_feature_define_nof
} dnx_data_oam_feature_define_e;




typedef enum
{

    
    _dnx_data_oam_feature_table_nof
} dnx_data_oam_feature_table_e;






shr_error_e dnx_data_oam_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

