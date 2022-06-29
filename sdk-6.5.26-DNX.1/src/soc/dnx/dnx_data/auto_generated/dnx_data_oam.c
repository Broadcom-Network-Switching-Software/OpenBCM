
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>




extern shr_error_e jr2_a0_data_oam_attach(
    int unit);


extern shr_error_e jr2_b0_data_oam_attach(
    int unit);


extern shr_error_e j2c_a0_data_oam_attach(
    int unit);


extern shr_error_e q2a_a0_data_oam_attach(
    int unit);


extern shr_error_e j2p_a0_data_oam_attach(
    int unit);


extern shr_error_e j2x_a0_data_oam_attach(
    int unit);




static shr_error_e
dnx_data_oam_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_oam_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam general features");

    submodule_data->features[dnx_data_oam_general_oam_not_supported].name = "oam_not_supported";
    submodule_data->features[dnx_data_oam_general_oam_not_supported].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_not_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].name = "oam_event_fifo_support";
    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_low_scaling].name = "low_scaling";
    submodule_data->features[dnx_data_oam_general_low_scaling].doc = "";
    submodule_data->features[dnx_data_oam_general_low_scaling].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].name = "max_oam_offset_support";
    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].doc = "";
    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_hlm_support].name = "oam_hlm_support";
    submodule_data->features[dnx_data_oam_general_oam_hlm_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_hlm_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_sub_type_twamp_tx_support].name = "oam_sub_type_twamp_tx_support";
    submodule_data->features[dnx_data_oam_general_oam_sub_type_twamp_tx_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_sub_type_twamp_tx_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_twamp_stateless_mode_support].name = "oam_twamp_stateless_mode_support";
    submodule_data->features[dnx_data_oam_general_oam_twamp_stateless_mode_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_twamp_stateless_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_dmac_mdl_match_check_control_support].name = "oam_dmac_mdl_match_check_control_support";
    submodule_data->features[dnx_data_oam_general_oam_dmac_mdl_match_check_control_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_dmac_mdl_match_check_control_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_lmdm_flexible_dmac_support].name = "oam_lmdm_flexible_dmac_support";
    submodule_data->features[dnx_data_oam_general_oam_lmdm_flexible_dmac_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_lmdm_flexible_dmac_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_lmdm_report_mode_support].name = "oam_lmdm_report_mode_support";
    submodule_data->features[dnx_data_oam_general_oam_lmdm_report_mode_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_lmdm_report_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_various_ep_memory_type_support].name = "oam_various_ep_memory_type_support";
    submodule_data->features[dnx_data_oam_general_oam_various_ep_memory_type_support].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_various_ep_memory_type_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_oam_twamp_flow_id_ext_enable].name = "oam_twamp_flow_id_ext_enable";
    submodule_data->features[dnx_data_oam_general_oam_twamp_flow_id_ext_enable].doc = "";
    submodule_data->features[dnx_data_oam_general_oam_twamp_flow_id_ext_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_oam_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam general defines");

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].name = "oam_nof_non_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].name = "oam_nof_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].name = "oam_nof_reserved_egr_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].name = "oam_egr_acc_action_profile_id_for_inject_mpls";
    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].name = "oam_nof_bits_non_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].name = "oam_nof_bits_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].name = "oam_nof_bits_oam_lif_db_key_base_ingress";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].name = "oam_nof_bits_oam_lif_db_key_base_egress";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].name = "oam_nof_endpoints";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].name = "oam_nof_oamp_meps";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].name = "oam_nof_groups";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].name = "oam_nof_entries_oam_lif_db";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].name = "oam_nof_bits_counter_base";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].name = "oam_nof_punt_error_codes";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].name = "oam_nof_priority_map_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].name = "oam_force_interface_for_meps_without_counter";
    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].name = "oam_my_cfm_mac_by_range";
    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].name = "oam_nof_pemla_channel_types";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].name = "oam_mdb_rmep_db_profile_limitation";
    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].name = "oam_lm_read_index_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].name = "oam_lm_read_per_sub_enable_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].name = "oam_lm_read_per_sub_disable_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].name = "oam_identification_appdb_1_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].name = "oam_ntp_freq_control_word";
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].name = "oam_ntp_time_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].name = "oam_IEEE_1588_time_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].name = "oam_nse_nco_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].name = "oam_nof_labels_for_sd_on_p_node";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].name = "oam_tod_config_in_eci";
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_ns].name = "oam_tod_config_in_ns";
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_ns].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_ns].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_local_outlif_msb_bit_index].name = "oam_local_outlif_msb_bit_index";
    submodule_data->defines[dnx_data_oam_general_define_oam_local_outlif_msb_bit_index].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_local_outlif_msb_bit_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_mpls_tp_or_bfd_enable_map_exists].name = "oam_mpls_tp_or_bfd_enable_map_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_mpls_tp_or_bfd_enable_map_exists].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_mpls_tp_or_bfd_enable_map_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_counter_interfaces].name = "oam_nof_counter_interfaces";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_counter_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_counter_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_sub_type_v2].name = "oam_sub_type_v2";
    submodule_data->defines[dnx_data_oam_general_define_oam_sub_type_v2].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_sub_type_v2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_server_device_profile].name = "oam_nof_server_device_profile";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_server_device_profile].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_server_device_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_app_id_init].name = "oam_app_id_init";
    submodule_data->defines[dnx_data_oam_general_define_oam_app_id_init].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_app_id_init].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_slm_dedicated_profile_supported].name = "oam_slm_dedicated_profile_supported";
    submodule_data->defines[dnx_data_oam_general_define_oam_slm_dedicated_profile_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_slm_dedicated_profile_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_tcam_key_pkt_data_at_fwd_header].name = "oam_tcam_key_pkt_data_at_fwd_header";
    submodule_data->defines[dnx_data_oam_general_define_oam_tcam_key_pkt_data_at_fwd_header].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_tcam_key_pkt_data_at_fwd_header].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_trap_action_profile_nof_bits].name = "oam_trap_action_profile_nof_bits";
    submodule_data->defines[dnx_data_oam_general_define_oam_trap_action_profile_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_trap_action_profile_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_oam_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data oam general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_oam_general_feature_get(
    int unit,
    dnx_data_oam_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, feature);
}


uint32
dnx_data_oam_general_oam_nof_non_acc_action_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_non_acc_action_profiles);
}

uint32
dnx_data_oam_general_oam_nof_acc_action_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_acc_action_profiles);
}

uint32
dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles);
}

uint32
dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls);
}

uint32
dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles);
}

uint32
dnx_data_oam_general_oam_nof_bits_acc_action_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles);
}

uint32
dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress);
}

uint32
dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress);
}

uint32
dnx_data_oam_general_oam_nof_endpoints_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_endpoints);
}

uint32
dnx_data_oam_general_oam_nof_oamp_meps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_oamp_meps);
}

uint32
dnx_data_oam_general_oam_nof_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_groups);
}

uint32
dnx_data_oam_general_oam_nof_entries_oam_lif_db_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_entries_oam_lif_db);
}

uint32
dnx_data_oam_general_oam_nof_bits_counter_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_bits_counter_base);
}

uint32
dnx_data_oam_general_oam_nof_punt_error_codes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_punt_error_codes);
}

uint32
dnx_data_oam_general_oam_nof_priority_map_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_priority_map_profiles);
}

uint32
dnx_data_oam_general_oam_force_interface_for_meps_without_counter_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter);
}

uint32
dnx_data_oam_general_oam_my_cfm_mac_by_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_my_cfm_mac_by_range);
}

uint32
dnx_data_oam_general_oam_nof_pemla_channel_types_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_pemla_channel_types);
}

uint32
dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation);
}

uint32
dnx_data_oam_general_oam_lm_read_index_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_lm_read_index_field_exists);
}

uint32
dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists);
}

uint32
dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists);
}

uint32
dnx_data_oam_general_oam_identification_appdb_1_field_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_identification_appdb_1_field_exists);
}

uint32
dnx_data_oam_general_oam_ntp_freq_control_word_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_ntp_freq_control_word);
}

uint32
dnx_data_oam_general_oam_ntp_time_freq_control_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_ntp_time_freq_control);
}

uint32
dnx_data_oam_general_oam_IEEE_1588_time_freq_control_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control);
}

uint32
dnx_data_oam_general_oam_nse_nco_freq_control_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nse_nco_freq_control);
}

uint32
dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node);
}

uint32
dnx_data_oam_general_oam_tod_config_in_eci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_tod_config_in_eci);
}

uint32
dnx_data_oam_general_oam_tod_config_in_ns_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_tod_config_in_ns);
}

uint32
dnx_data_oam_general_oam_local_outlif_msb_bit_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_local_outlif_msb_bit_index);
}

uint32
dnx_data_oam_general_oam_mpls_tp_or_bfd_enable_map_exists_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_mpls_tp_or_bfd_enable_map_exists);
}

uint32
dnx_data_oam_general_oam_nof_counter_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_counter_interfaces);
}

uint32
dnx_data_oam_general_oam_sub_type_v2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_sub_type_v2);
}

uint32
dnx_data_oam_general_oam_nof_server_device_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_nof_server_device_profile);
}

uint32
dnx_data_oam_general_oam_app_id_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_app_id_init);
}

uint32
dnx_data_oam_general_oam_slm_dedicated_profile_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_slm_dedicated_profile_supported);
}

uint32
dnx_data_oam_general_oam_tcam_key_pkt_data_at_fwd_header_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_tcam_key_pkt_data_at_fwd_header);
}

uint32
dnx_data_oam_general_oam_trap_action_profile_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_general, dnx_data_oam_general_define_oam_trap_action_profile_nof_bits);
}










static shr_error_e
dnx_data_oam_oamp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "oamp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_oam_oamp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam oamp features");

    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].name = "oamp_pe_support";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].doc = "";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].name = "oamp_pe_tx_machine_support";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].doc = "";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_oamp_oamp_v2].name = "oamp_v2";
    submodule_data->features[dnx_data_oam_oamp_oamp_v2].doc = "";
    submodule_data->features[dnx_data_oam_oamp_oamp_v2].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_oam_oamp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam oamp defines");

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].name = "nof_mep_db_entry_banks";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].name = "nof_mep_db_entries_per_bank";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].name = "scan_rate_bank_exist";
    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].name = "nof_mep_db_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].name = "max_nof_endpoint_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].name = "max_nof_mep_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].name = "max_value_of_mep_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].name = "nof_umc";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].name = "mep_id_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].name = "nof_mep_db_entry_banks_for_endpoints";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].name = "nof_mep_db_endpoint_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].name = "mep_db_da_is_mc_for_short_entries_support";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].name = "nof_icc_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].name = "local_port_2_system_port_size";
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].name = "oam_nof_sa_mac_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].name = "oam_nof_da_mac_msb_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].name = "oam_nof_da_mac_lsb_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].name = "oam_nof_tpid_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].name = "oamp_pe_default_instruction_index";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].name = "oamp_pe_max_nof_instructions";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].name = "oamp_pe_nof_fem_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].name = "mep_db_sub_entry_size";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].name = "mep_db_nof_sub_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].name = "mep_db_full_entry_size";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].name = "mep_pe_profile_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].name = "rmep_db_entries_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].name = "rmep_db_rmep_index_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].name = "rmep_max_self_contained";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_pair_sys_clocks].name = "rmep_pair_sys_clocks";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_pair_sys_clocks].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_pair_sys_clocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].name = "mep_sys_clocks";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_num_clocks_scan_nof_bits].name = "num_clocks_scan_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_num_clocks_scan_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_num_clocks_scan_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_us].name = "oamp_scan_sycles_in_us";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_us].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_us].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_ms].name = "oamp_scan_sycles_in_ms";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_ms].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_scan_sycles_in_ms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_version_limitation].name = "mep_db_version_limitation";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_version_limitation].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_version_limitation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation].name = "oamp_mep_db_threshold_version_limitation";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation].name = "rmep_db_non_zero_lifetime_units_limitation";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_crc_calculation_supported].name = "crc_calculation_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_crc_calculation_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_crc_calculation_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_modified_mep_db_structure].name = "modified_mep_db_structure";
    submodule_data->defines[dnx_data_oam_oamp_define_modified_mep_db_structure].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_modified_mep_db_structure].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].name = "local_port_2_system_port_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].name = "mdb_nof_access_ids";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].name = "bfd_mpls_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].name = "bfd_m_hop_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].name = "bfd_pwe_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].name = "ccm_eth_offload_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].name = "bfd_1_hop_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].name = "ccm_mpls_tp_pwe_offload_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].name = "punt_packet_destination_size";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].name = "punt_packet_itmh_destination_full_encoding_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].name = "core_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].name = "punt_packet_pph_ttl_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].name = "lmm_dmm_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].name = "slm_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_sat_rxi_enable_support].name = "oamp_sat_rxi_enable_support";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_sat_rxi_enable_support].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_sat_rxi_enable_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].name = "mdb_tx_rate_lsb_field_present";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].name = "nof_jumbo_dm_tlv_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].name = "mdb_reserved_field_size_calculation";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_event_fifo_slicing_size_offset].name = "event_fifo_slicing_size_offset";
    submodule_data->defines[dnx_data_oam_oamp_define_event_fifo_slicing_size_offset].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_event_fifo_slicing_size_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_ccm_piggyback_per_priority_with_crc_supported].name = "ccm_piggyback_per_priority_with_crc_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_piggyback_per_priority_with_crc_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_piggyback_per_priority_with_crc_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_write_complete_checks].name = "nof_write_complete_checks";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_write_complete_checks].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_write_complete_checks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_trap_code_tcam].name = "oam_nof_trap_code_tcam";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_trap_code_tcam].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_trap_code_tcam].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_rmep_db_access_regs].name = "mep_rmep_db_access_regs";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_rmep_db_access_regs].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_rmep_db_access_regs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_dev_type_for_server_tests].name = "dev_type_for_server_tests";
    submodule_data->defines[dnx_data_oam_oamp_define_dev_type_for_server_tests].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_dev_type_for_server_tests].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_header_size].name = "protection_packet_max_header_size";
    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_header_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_header_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_len_size].name = "protection_packet_max_len_size";
    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_len_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_protection_packet_max_len_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_number_of_events].name = "max_number_of_events";
    submodule_data->defines[dnx_data_oam_oamp_define_max_number_of_events].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_number_of_events].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_generic_memory_entries].name = "nof_generic_memory_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_generic_memory_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_generic_memory_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].name = "oamp_nof_scan_rmep_db_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].name = "oamp_mep_full_entry_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].name = "oamp_rmep_full_entry_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].name = "mep_db_jr1_mode";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].name = "oamp_fifo_dma_event_interface_enable";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].name = "oamp_fifo_dma_event_interface_buffer_size";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].name = "oamp_fifo_dma_event_interface_timeout";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].name = "oamp_fifo_dma_event_interface_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_thread_priority].name = "oamp_fifo_dma_event_thread_priority";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_thread_priority].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_thread_priority].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].name = "oamp_fifo_dma_report_interface_enable";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].name = "oamp_fifo_dma_report_interface_buffer_size";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].name = "oamp_fifo_dma_report_interface_timeout";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].name = "oamp_fifo_dma_report_interface_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_0].name = "up_mep_tod_recycle_port_core_0";
    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_0].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_1].name = "up_mep_tod_recycle_port_core_1";
    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_1].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].name = "server_destination_msb_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].name = "server_destination_lsb_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].name = "intr_oamp_pending_event";
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].name = "intr_oamp_stat_pending_event";
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_oam_oamp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data oam oamp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_oam_oamp_feature_get(
    int unit,
    dnx_data_oam_oamp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, feature);
}


uint32
dnx_data_oam_oamp_nof_mep_db_entry_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_mep_db_entry_banks);
}

uint32
dnx_data_oam_oamp_nof_mep_db_entries_per_bank_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank);
}

uint32
dnx_data_oam_oamp_scan_rate_bank_exist_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_scan_rate_bank_exist);
}

uint32
dnx_data_oam_oamp_nof_mep_db_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_mep_db_entries);
}

uint32
dnx_data_oam_oamp_max_nof_endpoint_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_max_nof_endpoint_id);
}

uint32
dnx_data_oam_oamp_max_nof_mep_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_max_nof_mep_id);
}

uint32
dnx_data_oam_oamp_max_value_of_mep_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_max_value_of_mep_id);
}

uint32
dnx_data_oam_oamp_nof_umc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_umc);
}

uint32
dnx_data_oam_oamp_mep_id_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_id_nof_bits);
}

uint32
dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints);
}

uint32
dnx_data_oam_oamp_nof_mep_db_endpoint_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries);
}

uint32
dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support);
}

uint32
dnx_data_oam_oamp_nof_icc_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_icc_profiles);
}

uint32
dnx_data_oam_oamp_local_port_2_system_port_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_local_port_2_system_port_size);
}

uint32
dnx_data_oam_oamp_oam_nof_sa_mac_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles);
}

uint32
dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles);
}

uint32
dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles);
}

uint32
dnx_data_oam_oamp_oam_nof_tpid_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oam_nof_tpid_profiles);
}

uint32
dnx_data_oam_oamp_oamp_pe_default_instruction_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_pe_default_instruction_index);
}

uint32
dnx_data_oam_oamp_oamp_pe_max_nof_instructions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions);
}

uint32
dnx_data_oam_oamp_oamp_pe_nof_fem_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits);
}

uint32
dnx_data_oam_oamp_mep_db_sub_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_sub_entry_size);
}

uint32
dnx_data_oam_oamp_mep_db_nof_sub_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_nof_sub_entries);
}

uint32
dnx_data_oam_oamp_mep_db_full_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_full_entry_size);
}

uint32
dnx_data_oam_oamp_mep_pe_profile_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_pe_profile_nof_bits);
}

uint32
dnx_data_oam_oamp_rmep_db_entries_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_db_entries_nof_bits);
}

uint32
dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits);
}

uint32
dnx_data_oam_oamp_rmep_max_self_contained_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_max_self_contained);
}

uint32
dnx_data_oam_oamp_rmep_pair_sys_clocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_pair_sys_clocks);
}

uint32
dnx_data_oam_oamp_mep_sys_clocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_sys_clocks);
}

uint32
dnx_data_oam_oamp_num_clocks_scan_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_num_clocks_scan_nof_bits);
}

uint32
dnx_data_oam_oamp_oamp_scan_sycles_in_us_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_scan_sycles_in_us);
}

uint32
dnx_data_oam_oamp_oamp_scan_sycles_in_ms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_scan_sycles_in_ms);
}

uint32
dnx_data_oam_oamp_mep_db_version_limitation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_version_limitation);
}

uint32
dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation);
}

uint32
dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation);
}

uint32
dnx_data_oam_oamp_crc_calculation_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_crc_calculation_supported);
}

uint32
dnx_data_oam_oamp_modified_mep_db_structure_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_modified_mep_db_structure);
}

uint32
dnx_data_oam_oamp_local_port_2_system_port_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_local_port_2_system_port_bits);
}

uint32
dnx_data_oam_oamp_mdb_nof_access_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mdb_nof_access_ids);
}

uint32
dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits);
}

uint32
dnx_data_oam_oamp_punt_packet_destination_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_punt_packet_destination_size);
}

uint32
dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported);
}

uint32
dnx_data_oam_oamp_core_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_core_nof_bits);
}

uint32
dnx_data_oam_oamp_punt_packet_pph_ttl_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported);
}

uint32
dnx_data_oam_oamp_lmm_dmm_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_lmm_dmm_supported);
}

uint32
dnx_data_oam_oamp_slm_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_slm_supported);
}

uint32
dnx_data_oam_oamp_oamp_sat_rxi_enable_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_sat_rxi_enable_support);
}

uint32
dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present);
}

uint32
dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries);
}

uint32
dnx_data_oam_oamp_mdb_reserved_field_size_calculation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation);
}

uint32
dnx_data_oam_oamp_event_fifo_slicing_size_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_event_fifo_slicing_size_offset);
}

uint32
dnx_data_oam_oamp_ccm_piggyback_per_priority_with_crc_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_ccm_piggyback_per_priority_with_crc_supported);
}

uint32
dnx_data_oam_oamp_nof_write_complete_checks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_write_complete_checks);
}

uint32
dnx_data_oam_oamp_oam_nof_trap_code_tcam_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oam_nof_trap_code_tcam);
}

uint32
dnx_data_oam_oamp_mep_rmep_db_access_regs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_rmep_db_access_regs);
}

uint32
dnx_data_oam_oamp_dev_type_for_server_tests_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_dev_type_for_server_tests);
}

uint32
dnx_data_oam_oamp_protection_packet_max_header_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_protection_packet_max_header_size);
}

uint32
dnx_data_oam_oamp_protection_packet_max_len_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_protection_packet_max_len_size);
}

uint32
dnx_data_oam_oamp_max_number_of_events_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_max_number_of_events);
}

uint32
dnx_data_oam_oamp_nof_generic_memory_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_nof_generic_memory_entries);
}

uint32
dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries);
}

uint32
dnx_data_oam_oamp_oamp_mep_full_entry_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold);
}

uint32
dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold);
}

uint32
dnx_data_oam_oamp_mep_db_jr1_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_db_jr1_mode);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_event_thread_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_event_thread_priority);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout);
}

uint32
dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold);
}

uint32
dnx_data_oam_oamp_up_mep_tod_recycle_port_core_0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_0);
}

uint32
dnx_data_oam_oamp_up_mep_tod_recycle_port_core_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_up_mep_tod_recycle_port_core_1);
}

uint32
dnx_data_oam_oamp_server_destination_msb_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_server_destination_msb_bits);
}

uint32
dnx_data_oam_oamp_server_destination_lsb_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_server_destination_lsb_bits);
}

uint32
dnx_data_oam_oamp_intr_oamp_pending_event_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_intr_oamp_pending_event);
}

uint32
dnx_data_oam_oamp_intr_oamp_stat_pending_event_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_intr_oamp_stat_pending_event);
}










static shr_error_e
dnx_data_oam_property_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "property";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_oam_property_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam property features");

    
    submodule_data->nof_defines = _dnx_data_oam_property_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam property defines");

    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].name = "oam_injected_over_lsp_cnt";
    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].doc = "";
    
    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_property_define_lm_dm_per_priority_enable].name = "lm_dm_per_priority_enable";
    submodule_data->defines[dnx_data_oam_property_define_lm_dm_per_priority_enable].doc = "";
    
    submodule_data->defines[dnx_data_oam_property_define_lm_dm_per_priority_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_oam_property_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data oam property tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_oam_property_feature_get(
    int unit,
    dnx_data_oam_property_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_property, feature);
}


uint32
dnx_data_oam_property_oam_injected_over_lsp_cnt_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_property, dnx_data_oam_property_define_oam_injected_over_lsp_cnt);
}

uint32
dnx_data_oam_property_lm_dm_per_priority_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_property, dnx_data_oam_property_define_lm_dm_per_priority_enable);
}










static shr_error_e
dnx_data_oam_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_oam_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam feature features");

    submodule_data->features[dnx_data_oam_feature_slr_scan_mashine_support].name = "slr_scan_mashine_support";
    submodule_data->features[dnx_data_oam_feature_slr_scan_mashine_support].doc = "";
    submodule_data->features[dnx_data_oam_feature_slr_scan_mashine_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_slm_j1_measure_support].name = "slm_j1_measure_support";
    submodule_data->features[dnx_data_oam_feature_slm_j1_measure_support].doc = "";
    submodule_data->features[dnx_data_oam_feature_slm_j1_measure_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].name = "oam_offset_supported";
    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oam_use_offset_supported].name = "oam_use_offset_supported";
    submodule_data->features[dnx_data_oam_feature_oam_use_offset_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oam_use_offset_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].name = "oam_statistics_supported";
    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].name = "lm_read_index_limitation";
    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_scanner_limitation].name = "oamp_scanner_limitation";
    submodule_data->features[dnx_data_oam_feature_oamp_scanner_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_scanner_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_tx_rate_limitation].name = "oamp_tx_rate_limitation";
    submodule_data->features[dnx_data_oam_feature_oamp_tx_rate_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_tx_rate_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_protection_rmep_id_limitation].name = "oamp_protection_rmep_id_limitation";
    submodule_data->features[dnx_data_oam_feature_oamp_protection_rmep_id_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_protection_rmep_id_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_generic_response_supported].name = "oamp_generic_response_supported";
    submodule_data->features[dnx_data_oam_feature_oamp_generic_response_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_generic_response_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_txm_bank_limitation].name = "oamp_txm_bank_limitation";
    submodule_data->features[dnx_data_oam_feature_oamp_txm_bank_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_txm_bank_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_rmep_scale_limitataion].name = "oamp_rmep_scale_limitataion";
    submodule_data->features[dnx_data_oam_feature_oamp_rmep_scale_limitataion].doc = "";
    submodule_data->features[dnx_data_oam_feature_oamp_rmep_scale_limitataion].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_up_mep_tod_recycle_enable].name = "up_mep_tod_recycle_enable";
    submodule_data->features[dnx_data_oam_feature_up_mep_tod_recycle_enable].doc = "";
    submodule_data->features[dnx_data_oam_feature_up_mep_tod_recycle_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_pcp_based_lm_dm_support].name = "pcp_based_lm_dm_support";
    submodule_data->features[dnx_data_oam_feature_pcp_based_lm_dm_support].doc = "";
    submodule_data->features[dnx_data_oam_feature_pcp_based_lm_dm_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_ptch_oam_offset_limitation].name = "ptch_oam_offset_limitation";
    submodule_data->features[dnx_data_oam_feature_ptch_oam_offset_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_ptch_oam_offset_limitation].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_oam_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam feature defines");

    
    submodule_data->nof_tables = _dnx_data_oam_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data oam feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_oam_feature_feature_get(
    int unit,
    dnx_data_oam_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_feature, feature);
}








shr_error_e
dnx_data_oam_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "oam";
    module_data->nof_submodules = _dnx_data_oam_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data oam submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_oam_general_init(unit, &module_data->submodules[dnx_data_oam_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_oam_oamp_init(unit, &module_data->submodules[dnx_data_oam_submodule_oamp]));
    SHR_IF_ERR_EXIT(dnx_data_oam_property_init(unit, &module_data->submodules[dnx_data_oam_submodule_property]));
    SHR_IF_ERR_EXIT(dnx_data_oam_feature_init(unit, &module_data->submodules[dnx_data_oam_submodule_feature]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_oam_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_oam_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_oam_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

