

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



static shr_error_e
dnx_data_oam_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general oam constants";
    
    submodule_data->nof_features = _dnx_data_oam_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam general features");

    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].name = "oam_event_fifo_support";
    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].doc = "Indicate if OAM event FIFO is supported";
    submodule_data->features[dnx_data_oam_general_oam_event_fifo_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_low_scaling].name = "low_scaling";
    submodule_data->features[dnx_data_oam_general_low_scaling].doc = "Indicate if low scaling is present";
    submodule_data->features[dnx_data_oam_general_low_scaling].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].name = "max_oam_offset_support";
    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].doc = "Indicate if the max OAM offset filter is supported";
    submodule_data->features[dnx_data_oam_general_max_oam_offset_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_oam_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam general defines");

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].name = "oam_nof_non_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].doc = "Number of profiles for non-accelerated OAM endpoints";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_non_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].name = "oam_nof_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].doc = "Number of profiles for accelerated OAM endpoints";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].name = "oam_nof_reserved_egr_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].doc = "Number of reserved egress profiles for accelerated OAM endpoints";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].name = "oam_egr_acc_action_profile_id_for_inject_mpls";
    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].doc = "ID of Egress acc egress profile for injected mpls oam";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].name = "oam_nof_bits_non_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].doc = "Number of bits needed for profiles for non-accelerated OAM endpoints";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].name = "oam_nof_bits_acc_action_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].doc = "Number of bits needed for profiles for accelerated OAM endpoints";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].name = "oam_nof_bits_oam_lif_db_key_base_ingress";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].doc = "Number of bits used in ingress LIF DB and Acc MEP DB tables for LIF ID key";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].name = "oam_nof_bits_oam_lif_db_key_base_egress";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].doc = "Number of bits used in egress LIF DB and Acc MEP DB tables for LIF ID key";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].name = "oam_nof_endpoints";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].doc = "Number of OAM endpoints that may be allocated";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].name = "oam_nof_oamp_meps";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].doc = "Number of endpoints that may be defined in the OAMP.";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_oamp_meps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].name = "oam_nof_groups";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].doc = "Number of oam groups in the device";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].name = "oam_nof_entries_oam_lif_db";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].doc = "Number of OAM lifs that may be defined";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_entries_oam_lif_db].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].name = "oam_nof_bits_counter_base";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].doc = "Number of bits in counter base field in OAM-LIF-DB payload";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_bits_counter_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].name = "oam_nof_punt_error_codes";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].doc = "Number of error codes supported by punt feature";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_punt_error_codes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].name = "oam_nof_priority_map_profiles";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].doc = "Number of priority map profiles supported (egress tc to pcp mapping)";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_priority_map_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].name = "oam_force_interface_for_meps_without_counter";
    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].doc = "";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].name = "oam_my_cfm_mac_by_range";
    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].doc = "My CFM MAC defined by range rather than bitmap";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_my_cfm_mac_by_range].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].name = "oam_nof_pemla_channel_types";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].doc = "number of channel types in PEMLA OAM";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_pemla_channel_types].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].name = "oam_mdb_rmep_db_profile_limitation";
    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].doc = "Minimum capacity for RMEP_DB in MDB profile ";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].name = "oam_lm_read_index_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].doc = "1 if current device has this field; 0 if it does not have it";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_index_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].name = "oam_lm_read_per_sub_enable_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].doc = "1 if current device has this field; 0 if it does not have it";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].name = "oam_lm_read_per_sub_disable_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].doc = "1 if current device has this field; 0 if it does not have it";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].name = "oam_identification_appdb_1_field_exists";
    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].doc = "1 if current device has this field; 0 if it does not have it";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_identification_appdb_1_field_exists].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].name = "oam_ntp_freq_control_word";
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].doc = "NTP Frequency control word";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_freq_control_word].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].name = "oam_ntp_time_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].doc = "Frequency value added to the 32 bit accumulator";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_ntp_time_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].name = "oam_IEEE_1588_time_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].doc = "Frequency value added to the 32 bit accumulator";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].name = "oam_nse_nco_freq_control";
    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].doc = "Determines whether the NCO Frequency stepping control is from a register or DPLL";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nse_nco_freq_control].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].name = "oam_nof_labels_for_sd_on_p_node";
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].doc = "Numbers of MPLS labels for transmit CCM SD P-node flow with.";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].name = "oam_tod_config_in_eci";
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].doc = "Determines whether the ToD Configuration is done through ECI registers";
    
    submodule_data->defines[dnx_data_oam_general_define_oam_tod_config_in_eci].flags |= DNXC_DATA_F_DEFINE;

    
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










static shr_error_e
dnx_data_oam_oamp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "oamp";
    submodule_data->doc = "oamp related consts";
    
    submodule_data->nof_features = _dnx_data_oam_oamp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam oamp features");

    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].name = "oamp_pe_support";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].doc = "Indicate if OAMP PE is supported";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].name = "oamp_pe_tx_machine_support";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].doc = "Indicate if OAMP PE tx machine is supported";
    submodule_data->features[dnx_data_oam_oamp_oamp_pe_tx_machine_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_oam_oamp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam oamp defines");

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].name = "nof_mep_db_entry_banks";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].doc = "Number of entry banks for OAMP MEP DB";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].name = "nof_mep_db_entries_per_bank";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].doc = "Number of entries per bank for OAMP MEP DB";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].name = "scan_rate_bank_exist";
    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].doc = "In JER_A0 different scan rate not supported.";
    
    submodule_data->defines[dnx_data_oam_oamp_define_scan_rate_bank_exist].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].name = "nof_mep_db_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].doc = "Total number of entries in OAMP MEP DB";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].name = "max_nof_endpoint_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].doc = "Maximal number of endpoints ID that could be created";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_endpoint_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].name = "max_nof_mep_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].doc = "Maiximal number of possible MEP-ID's(size of MEP_DB table).";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_nof_mep_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].name = "max_value_of_mep_id";
    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].doc = "Maximal value of OAM ID (upper limit of table key)";
    
    submodule_data->defines[dnx_data_oam_oamp_define_max_value_of_mep_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].name = "nof_umc";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].doc = "Number of maximum Unique MEG-ID Codes";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_umc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].name = "mep_id_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].doc = "Number of bits in 'OAM_ID' of an OAM/BFD accelerated endpoint";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_id_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].name = "nof_mep_db_entry_banks_for_endpoints";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].doc = "Number of entry banks in OAMP MEP DB that can contain endpoints";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].name = "nof_mep_db_endpoint_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].doc = "Number of entries in OAMP MEP DB that can contain endpoints";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].name = "mep_db_da_is_mc_for_short_entries_support";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].name = "nof_icc_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].doc = "Number of ICC profiles in OAMP";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_icc_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].name = "local_port_2_system_port_size";
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].doc = "Size of local to system port table";
    
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].name = "oam_nof_sa_mac_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].doc = "Number of sa mac profiles(33msb)";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].name = "oam_nof_da_mac_msb_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].doc = "Number of da mac msb profiles";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].name = "oam_nof_da_mac_lsb_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].doc = "Number of da mac lsb profiles";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].name = "oam_nof_tpid_profiles";
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].doc = "Number of tpid profiles";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oam_nof_tpid_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].name = "oamp_pe_default_instruction_index";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].doc = "index of default instruction";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_default_instruction_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].name = "oamp_pe_max_nof_instructions";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].doc = "maximal number of instructions to execute in single program";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].name = "oamp_pe_nof_fem_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].doc = "number of bits that each FEM configures";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].name = "mep_db_sub_entry_size";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].doc = "Number of bits in OAMP MEP DB sub-entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_sub_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].name = "mep_db_nof_sub_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].doc = "Number of sub-entries each full entry in OAMP MEP DB";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_nof_sub_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].name = "mep_db_full_entry_size";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].doc = "Number of bits in OAMP MEP DB full entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_full_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].name = "mep_pe_profile_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].doc = "Size of MEP PE profile";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_pe_profile_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].name = "rmep_db_entries_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].doc = "Number of bits used for remote MEP";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_entries_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].name = "rmep_db_rmep_index_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].doc = "Number of bits used for RMEP index";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].name = "rmep_max_self_contained";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].doc = "Number of maximum self contained RMEP's";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_max_self_contained].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_rmep_sys_clocks].name = "rmep_sys_clocks";
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_sys_clocks].doc = "Number of system clocks required for single RMEP scan";
    
    submodule_data->defines[dnx_data_oam_oamp_define_rmep_sys_clocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].name = "mep_sys_clocks";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].doc = "Number of system clocks required for single MEP scan";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_sys_clocks].flags |= DNXC_DATA_F_DEFINE;

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
    submodule_data->defines[dnx_data_oam_oamp_define_modified_mep_db_structure].doc = "Differntiates betweeen JR2 A0 structure and B0 and up structure";
    
    submodule_data->defines[dnx_data_oam_oamp_define_modified_mep_db_structure].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].name = "local_port_2_system_port_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].doc = "Number of bits in local to system port field";
    
    submodule_data->defines[dnx_data_oam_oamp_define_local_port_2_system_port_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].name = "mdb_nof_access_ids";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].doc = "number of access ids per mdb entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_nof_access_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].name = "bfd_mpls_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].doc = "Number of bits in reserved field in mdb bfd over MPLS 1/4 entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].name = "bfd_m_hop_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].doc = "Number of bits in reserved field in mdb bfd over multi-hop IPv4 1/4 entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].name = "bfd_pwe_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].doc = "Number of bits in reserved field in mdb bfd over PWE 1/4 entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].name = "ccm_eth_offload_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].doc = "Number of bits reserved field in mdb CCM ethernet LM/DM offloaded entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].name = "bfd_1_hop_short_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].doc = "Number of bits in reserved field in mdb bfd over single-hop IPv4 1/4 entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].name = "ccm_mpls_tp_pwe_offload_mdb_reserved_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].doc = "Number of bits reserved field in mdb CCM over MPLS-TP/PWE LM/DM offloaded entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].name = "punt_packet_destination_size";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].doc = "Number of bits in the destination fields in the OAMP_CPUPORT register";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_destination_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].name = "punt_packet_itmh_destination_full_encoding_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].doc = "Due to HW issue less encodings are avaliable in JER_A0/B0";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].name = "core_nof_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].doc = "Number of core bits in dbal field";
    
    submodule_data->defines[dnx_data_oam_oamp_define_core_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].name = "punt_packet_pph_ttl_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].name = "lmm_dmm_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].doc = "";
    
    submodule_data->defines[dnx_data_oam_oamp_define_lmm_dmm_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].name = "intr_oamp_pending_event";
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].doc = "interrupt index - oamp pending event";
    
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_pending_event].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].name = "intr_oamp_stat_pending_event";
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].doc = "interrupt index - oamp stat pending event";
    
    submodule_data->defines[dnx_data_oam_oamp_define_intr_oamp_stat_pending_event].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].name = "slm_supported";
    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].doc = "SLM has new mechanism in JR2_B0 and above!";
    
    submodule_data->defines[dnx_data_oam_oamp_define_slm_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].name = "mdb_tx_rate_lsb_field_present";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].doc = "Indicates if tx rate least significant bit is part of the MDB entry";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].name = "nof_jumbo_dm_tlv_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].doc = "The max number of dm tlv entries";
    
    submodule_data->defines[dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].name = "mdb_reserved_field_size_calculation";
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].doc = "Used in calculation of reservevd field size for MDB entries";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].name = "oamp_nof_scan_rmep_db_entries";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].doc = "Number of remote MEPs";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].name = "oamp_mep_full_entry_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].doc = "MEP DB Full Entry Threshold";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].name = "oamp_rmep_full_entry_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].doc = "RMEP DB Full Entry Threshold";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].name = "mep_db_jr1_mode";
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].doc = "MEP_DB mode(Inter-op)";
    
    submodule_data->defines[dnx_data_oam_oamp_define_mep_db_jr1_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].name = "oamp_fifo_dma_event_interface_enable";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].doc = "Enables OAMP DMA event interrupts interface";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].name = "oamp_fifo_dma_event_interface_buffer_size";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].doc = "OAMP fifo DMA host memory buffer size in bytes";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].name = "oamp_fifo_dma_event_interface_timeout";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].doc = "OAMP fifo dma timeout in microseconds";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].name = "oamp_fifo_dma_event_interface_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].doc = "OAMP fifo dma threshold to raise an interrupt.";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].name = "oamp_fifo_dma_report_interface_enable";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].doc = "Enables OAMP DMA statistic interrupts interface";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].name = "oamp_fifo_dma_report_interface_buffer_size";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].doc = "OAMP fifo DMA statistic host memory buffer size in bytes";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].name = "oamp_fifo_dma_report_interface_timeout";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].doc = "OAMP fifo dma statistic timeout in microseconds";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].name = "oamp_fifo_dma_report_interface_threshold";
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].doc = "OAMP fifo dma statistic threshold to raise an interrupt.";
    
    submodule_data->defines[dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].name = "server_destination_msb_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].doc = "Server destinaion: number of bits stored in MSB part";
    
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_msb_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].name = "server_destination_lsb_bits";
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].doc = "Server destinaion: number of bits stored in LSB part";
    
    submodule_data->defines[dnx_data_oam_oamp_define_server_destination_lsb_bits].flags |= DNXC_DATA_F_NUMERIC;

    
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
dnx_data_oam_oamp_rmep_sys_clocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_rmep_sys_clocks);
}

uint32
dnx_data_oam_oamp_mep_sys_clocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_mep_sys_clocks);
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

uint32
dnx_data_oam_oamp_slm_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_oam, dnx_data_oam_submodule_oamp, dnx_data_oam_oamp_define_slm_supported);
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










static shr_error_e
dnx_data_oam_property_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "property";
    submodule_data->doc = "customer soc property";
    
    submodule_data->nof_features = _dnx_data_oam_property_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data oam property features");

    
    submodule_data->nof_defines = _dnx_data_oam_property_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data oam property defines");

    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].name = "oam_injected_over_lsp_cnt";
    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].doc = "Count injected CCM packet over LSP";
    
    submodule_data->defines[dnx_data_oam_property_define_oam_injected_over_lsp_cnt].flags |= DNXC_DATA_F_NUMERIC;

    
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
    submodule_data->features[dnx_data_oam_feature_slr_scan_mashine_support].doc = "Indicate if the Txm for SLR measuring is supported";
    submodule_data->features[dnx_data_oam_feature_slr_scan_mashine_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].name = "oam_offset_supported";
    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oam_offset_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].name = "oam_statistics_supported";
    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].doc = "";
    submodule_data->features[dnx_data_oam_feature_oam_statistics_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].name = "lm_read_index_limitation";
    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].doc = "";
    submodule_data->features[dnx_data_oam_feature_lm_read_index_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_oam_feature_oamp_scanner_limitation].name = "oamp_scanner_limitation";
    submodule_data->features[dnx_data_oam_feature_oamp_scanner_limitation].doc = "RMEP is unreliable in A0 device";
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

