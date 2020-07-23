

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>



extern shr_error_e jr2_a0_data_l2_attach(
    int unit);
extern shr_error_e jr2_b0_data_l2_attach(
    int unit);
extern shr_error_e j2c_a0_data_l2_attach(
    int unit);
extern shr_error_e q2a_a0_data_l2_attach(
    int unit);
extern shr_error_e q2a_b1_data_l2_attach(
    int unit);
extern shr_error_e j2p_a0_data_l2_attach(
    int unit);



static shr_error_e
dnx_data_l2_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General L2 properties";
    
    submodule_data->nof_features = _dnx_data_l2_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 general features");

    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].name = "learning_use_insert_cmd";
    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].doc = "If set, learn events to MACT will be generated with INSERT command";
    submodule_data->features[dnx_data_l2_general_learning_use_insert_cmd].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_flush_machine_support].name = "flush_machine_support";
    submodule_data->features[dnx_data_l2_general_flush_machine_support].doc = "Indicate if flush machine is supported";
    submodule_data->features[dnx_data_l2_general_flush_machine_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].name = "dynamic_entries_iteration_support";
    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].doc = "Indicate if iterating over dynamic entries is supported";
    submodule_data->features[dnx_data_l2_general_dynamic_entries_iteration_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_counters_support].name = "counters_support";
    submodule_data->features[dnx_data_l2_general_counters_support].doc = "Indicate if l2 counters are supported";
    submodule_data->features[dnx_data_l2_general_counters_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_general_aging_support].name = "aging_support";
    submodule_data->features[dnx_data_l2_general_aging_support].doc = "Indicate if aging of l2 entries is supported";
    submodule_data->features[dnx_data_l2_general_aging_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 general defines");

    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].name = "vsi_offset_shift";
    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_vsi_offset_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].name = "lif_offset_shift";
    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].doc = "";
    
    submodule_data->defines[dnx_data_l2_general_define_lif_offset_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].name = "l2_learn_limit_mode";
    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].doc = "MACT learning limit mode";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_learn_limit_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].name = "jr_mode_nof_fec_bits";
    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].doc = "NOF FEC bits in Jericho DSP learning message";
    
    submodule_data->defines[dnx_data_l2_general_define_jr_mode_nof_fec_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].name = "arad_plus_mode_nof_fec_bits";
    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].doc = "NOF FEC bits in Arad+ DSP learning message";
    
    submodule_data->defines[dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].name = "l2_egress_max_extention_size_bytes";
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].doc = "Maximum size in bytes of RCH extensiuon. The limit comes from parser limitations";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_extention_size_bytes].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].name = "l2_egress_max_additional_termination_size_bytes";
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].doc = "Maximum size in bytes of additional termination on Egress to Parsing Start Offset";
    
    submodule_data->defines[dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_general_feature_get(
    int unit,
    dnx_data_l2_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, feature);
}


uint32
dnx_data_l2_general_vsi_offset_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_vsi_offset_shift);
}

uint32
dnx_data_l2_general_lif_offset_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_lif_offset_shift);
}

uint32
dnx_data_l2_general_l2_learn_limit_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_learn_limit_mode);
}

uint32
dnx_data_l2_general_jr_mode_nof_fec_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_jr_mode_nof_fec_bits);
}

uint32
dnx_data_l2_general_arad_plus_mode_nof_fec_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits);
}

uint32
dnx_data_l2_general_l2_egress_max_extention_size_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_egress_max_extention_size_bytes);
}

uint32
dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_general, dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes);
}










static shr_error_e
dnx_data_l2_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_l2_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 feature features");

    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].name = "age_out_and_refresh_profile_selection";
    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].doc = "Age out and refresh distribution is always taken from the default profile";
    submodule_data->features[dnx_data_l2_feature_age_out_and_refresh_profile_selection].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].name = "age_machine_pause_after_flush";
    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].doc = "Age machine stops working after flush runs";
    submodule_data->features[dnx_data_l2_feature_age_machine_pause_after_flush].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].name = "age_state_not_updated";
    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].doc = "Age state of l2 entries is not updated";
    submodule_data->features[dnx_data_l2_feature_age_state_not_updated].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].name = "appdb_id_for_olp";
    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].doc = "AppDB ID is wrongly taken from the 4 MSBs like in Jericho";
    submodule_data->features[dnx_data_l2_feature_appdb_id_for_olp].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].name = "eth_qual_is_mc";
    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].doc = "Parser Ethernet qualifier is_mc is set correctly";
    submodule_data->features[dnx_data_l2_feature_eth_qual_is_mc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].name = "bc_same_as_unknown_mc";
    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].doc = "";
    submodule_data->features[dnx_data_l2_feature_bc_same_as_unknown_mc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].name = "fid_mgmt_ecc_error";
    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].doc = "";
    submodule_data->features[dnx_data_l2_feature_fid_mgmt_ecc_error].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].name = "wrong_limit_interrupt_handling";
    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].doc = "No interrupt is raised for fid limit cross";
    submodule_data->features[dnx_data_l2_feature_wrong_limit_interrupt_handling].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].name = "transplant_instead_of_refresh";
    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].doc = "Transplant event is created instead of refresh event";
    submodule_data->features[dnx_data_l2_feature_transplant_instead_of_refresh].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].name = "static_mac_age_out";
    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].doc = "Static mac entries age out and deleted";
    submodule_data->features[dnx_data_l2_feature_static_mac_age_out].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].name = "vmv_for_limit_in_wrong_location";
    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].doc = "For limit check the vmv is taken from the wrong side of the mdb entry";
    submodule_data->features[dnx_data_l2_feature_vmv_for_limit_in_wrong_location].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_limit].name = "learn_limit";
    submodule_data->features[dnx_data_l2_feature_learn_limit].doc = "L2 limit causes supression of events related to existing entries (eg transplant/refresh)";
    submodule_data->features[dnx_data_l2_feature_learn_limit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].name = "limit_per_lif_counters";
    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].doc = "";
    submodule_data->features[dnx_data_l2_feature_limit_per_lif_counters].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].name = "learn_events_wrong_command";
    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].doc = "";
    submodule_data->features[dnx_data_l2_feature_learn_events_wrong_command].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].name = "opportunistic_learning_always_transplant";
    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].doc = "";
    submodule_data->features[dnx_data_l2_feature_opportunistic_learning_always_transplant].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].name = "exceed_limit_interrupt_by_insert_cmd";
    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].doc = "";
    submodule_data->features[dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].name = "refresh_events_wrong_key_msbs";
    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].doc = "";
    submodule_data->features[dnx_data_l2_feature_refresh_events_wrong_key_msbs].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].name = "ignore_limit_check";
    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].doc = "";
    submodule_data->features[dnx_data_l2_feature_ignore_limit_check].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 feature defines");

    
    submodule_data->nof_tables = _dnx_data_l2_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_feature_feature_get(
    int unit,
    dnx_data_l2_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_feature, feature);
}











static shr_error_e
dnx_data_l2_vsi_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vsi";
    submodule_data->doc = "VSI data";
    
    submodule_data->nof_features = _dnx_data_l2_vsi_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 vsi features");

    
    submodule_data->nof_defines = _dnx_data_l2_vsi_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 vsi defines");

    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].name = "nof_vsi_aging_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].doc = "Number of VSI aging profiles";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_aging_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].name = "nof_event_forwarding_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].doc = "Number of event forwarding profiles";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_event_forwarding_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].name = "nof_vsi_learning_profiles";
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].doc = "Number of VSI learning profiles";
    
    submodule_data->defines[dnx_data_l2_vsi_define_nof_vsi_learning_profiles].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_vsi_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 vsi tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_vsi_feature_get(
    int unit,
    dnx_data_l2_vsi_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, feature);
}


uint32
dnx_data_l2_vsi_nof_vsi_aging_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_vsi_aging_profiles);
}

uint32
dnx_data_l2_vsi_nof_event_forwarding_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_event_forwarding_profiles);
}

uint32
dnx_data_l2_vsi_nof_vsi_learning_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vsi, dnx_data_l2_vsi_define_nof_vsi_learning_profiles);
}










static shr_error_e
dnx_data_l2_vlan_domain_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vlan_domain";
    submodule_data->doc = "vlan domain";
    
    submodule_data->nof_features = _dnx_data_l2_vlan_domain_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 vlan_domain features");

    
    submodule_data->nof_defines = _dnx_data_l2_vlan_domain_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 vlan_domain defines");

    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].name = "nof_vlan_domains";
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].doc = "Number of supported vlan domains";
    
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_vlan_domains].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].name = "nof_bits_next_layer_network_domain";
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].doc = "Number of bits for next layer network domain";
    
    submodule_data->defines[dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_vlan_domain_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 vlan_domain tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_vlan_domain_feature_get(
    int unit,
    dnx_data_l2_vlan_domain_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, feature);
}


uint32
dnx_data_l2_vlan_domain_nof_vlan_domains_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, dnx_data_l2_vlan_domain_define_nof_vlan_domains);
}

uint32
dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_vlan_domain, dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain);
}










static shr_error_e
dnx_data_l2_dma_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dma";
    submodule_data->doc = "dma";
    
    submodule_data->nof_features = _dnx_data_l2_dma_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 dma features");

    
    submodule_data->nof_defines = _dnx_data_l2_dma_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 dma defines");

    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].name = "flush_nof_dma_entries";
    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].doc = "Number of flush DMA entries that the host can hold";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_nof_dma_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].name = "flush_db_nof_dma_rules";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].doc = "Number of flush DMA rules";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].name = "flush_db_nof_dma_rules_per_table";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].doc = "Number of flush DMA rules in a table";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].name = "flush_db_rule_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].doc = "Size in bytes of a flush DB rule";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_rule_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].name = "flush_db_data_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].doc = "Size in bytes of a flush DB action data";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_db_data_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].name = "flush_group_size";
    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].doc = "the size of flush group field";
    
    submodule_data->defines[dnx_data_l2_dma_define_flush_group_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].name = "learning_fifo_dma_buffer_size";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].doc = "learning fifo dma buffer size in bytes";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_buffer_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].name = "learning_fifo_dma_timeout";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].doc = "learning fifo dma timeout in microseconds";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_timeout].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].name = "learning_fifo_dma_threshold";
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].doc = "learning fifo dma threshold";
    
    submodule_data->defines[dnx_data_l2_dma_define_learning_fifo_dma_threshold].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_l2_dma_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 dma tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_dma_feature_get(
    int unit,
    dnx_data_l2_dma_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, feature);
}


uint32
dnx_data_l2_dma_flush_nof_dma_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_nof_dma_entries);
}

uint32
dnx_data_l2_dma_flush_db_nof_dma_rules_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_nof_dma_rules);
}

uint32
dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table);
}

uint32
dnx_data_l2_dma_flush_db_rule_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_rule_size);
}

uint32
dnx_data_l2_dma_flush_db_data_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_db_data_size);
}

uint32
dnx_data_l2_dma_flush_group_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_flush_group_size);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_buffer_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_buffer_size);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_timeout_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_timeout);
}

uint32
dnx_data_l2_dma_learning_fifo_dma_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_dma, dnx_data_l2_dma_define_learning_fifo_dma_threshold);
}










static shr_error_e
dnx_data_l2_age_and_flush_machine_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "age_and_flush_machine";
    submodule_data->doc = "Scan machine used for flush and age operations.";
    
    submodule_data->nof_features = _dnx_data_l2_age_and_flush_machine_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 age_and_flush_machine features");

    
    submodule_data->nof_defines = _dnx_data_l2_age_and_flush_machine_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 age_and_flush_machine defines");

    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].name = "max_age_states";
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].doc = "The maximal NOF age states that an entry can have before aging out.";
    
    submodule_data->defines[dnx_data_l2_age_and_flush_machine_define_max_age_states].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_age_and_flush_machine_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 age_and_flush_machine tables");

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].name = "filter_rules";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].doc = "Holds flush filter rules";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_filter_rules_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].entry_get = dnx_data_l2_age_and_flush_machine_filter_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].nof_values, "_dnx_data_l2_age_and_flush_machine_table_filter_rules table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, table_name);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[1].name = "valid";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[1].doc = "1 means rule is valid and should be used. 0 rule should not be checked.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, valid);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[2].name = "entry_mask";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[2].doc = "Filter rule for entry mask. 1 is don't care and 0 means compare.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, entry_mask);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[3].name = "entry_filter";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[3].doc = "Filter rule for entry.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, entry_filter);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[4].name = "src_mask";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[4].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[4].doc = "Filter rule for src mask.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[4].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, src_mask);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[5].name = "src_filter";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[5].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[5].doc = "Filter rule for src.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[5].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, src_filter);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[6].name = "appdb_id_mask";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[6].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[6].doc = "Filter rule for appdb id mask.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[6].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, appdb_id_mask);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[7].name = "appdb_id_filter";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[7].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[7].doc = "Filter rule for appdb id.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[7].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, appdb_id_filter);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[8].name = "accessed_mask";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[8].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[8].doc = "Filter rule for accessed mask.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[8].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, accessed_mask);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[9].name = "accessed_filter";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[9].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[9].doc = "Filter rule for accessed bit.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_filter_rules].values[9].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_filter_rules_t, accessed_filter);

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].name = "data_rules";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].doc = "Holds flush data for updates";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_data_rules_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].entry_get = dnx_data_l2_age_and_flush_machine_data_rules_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].nof_values, "_dnx_data_l2_age_and_flush_machine_table_data_rules table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[0].name = "table_name";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[0].doc = "memory name";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_data_rules_t, table_name);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[1].name = "command";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[1].doc = "command type. 0-clear hit, 1-get, 2-delete, 3-transplant";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_data_rules_t, command);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[2].name = "payload_mask";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[2].doc = "Mask for the payload in transplant action.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[2].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_data_rules_t, payload_mask);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[3].name = "payload";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[3].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[3].doc = "Payload for transplant action.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_data_rules].values[3].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_data_rules_t, payload);

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].name = "flush";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].doc = "Flush machine related HW";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_flush_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].entry_get = dnx_data_l2_age_and_flush_machine_flush_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].nof_values, "_dnx_data_l2_age_and_flush_machine_table_flush table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[0].name = "flush_pulse";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[0].doc = "Pulse the flush machine to start scanning.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_flush_t, flush_pulse);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[1].name = "traverse_thread_priority";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[1].type = "uint32";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[1].doc = "L2 flush match traverse non-blocking thread priority.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_flush].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_flush_t, traverse_thread_priority);

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].name = "age";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].doc = "Age machine related HW";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].size_of_values = sizeof(dnx_data_l2_age_and_flush_machine_age_t);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].entry_get = dnx_data_l2_age_and_flush_machine_age_entry_str_get;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_keys = 0;

    
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values, dnxc_data_value_t, submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].nof_values, "_dnx_data_l2_age_and_flush_machine_table_age table values");
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].name = "age_config";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].doc = "Age configuration register";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[0].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, age_config);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].name = "disable_aging";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].doc = "Disable aging.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[1].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, disable_aging);
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].name = "scan_pulse";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].type = "soc_reg_t";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].doc = "Pulse the scan machine to start scanning.";
    submodule_data->tables[dnx_data_l2_age_and_flush_machine_table_age].values[2].offset = UTILEX_OFFSETOF(dnx_data_l2_age_and_flush_machine_age_t, scan_pulse);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_age_and_flush_machine_feature_get(
    int unit,
    dnx_data_l2_age_and_flush_machine_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, feature);
}


uint32
dnx_data_l2_age_and_flush_machine_max_age_states_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_define_max_age_states);
}



const dnx_data_l2_age_and_flush_machine_filter_rules_t *
dnx_data_l2_age_and_flush_machine_filter_rules_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_filter_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l2_age_and_flush_machine_filter_rules_t *) data;

}

const dnx_data_l2_age_and_flush_machine_data_rules_t *
dnx_data_l2_age_and_flush_machine_data_rules_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_data_rules);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l2_age_and_flush_machine_data_rules_t *) data;

}

const dnx_data_l2_age_and_flush_machine_flush_t *
dnx_data_l2_age_and_flush_machine_flush_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l2_age_and_flush_machine_flush_t *) data;

}

const dnx_data_l2_age_and_flush_machine_age_t *
dnx_data_l2_age_and_flush_machine_age_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_l2_age_and_flush_machine_age_t *) data;

}


shr_error_e
dnx_data_l2_age_and_flush_machine_filter_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_filter_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_filter_rules);
    data = (const dnx_data_l2_age_and_flush_machine_filter_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_mask);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_filter);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_mask);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->src_filter);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_mask);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->appdb_id_filter);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_mask);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->accessed_filter);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_age_and_flush_machine_data_rules_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_data_rules_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_data_rules);
    data = (const dnx_data_l2_age_and_flush_machine_data_rules_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->command);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload_mask);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->payload);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_age_and_flush_machine_flush_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_flush_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush);
    data = (const dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flush_pulse);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->traverse_thread_priority);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_l2_age_and_flush_machine_age_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_l2_age_and_flush_machine_age_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);
    data = (const dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->age_config);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->disable_aging);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->scan_pulse);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_filter_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_filter_rules);

}

const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_data_rules_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_data_rules);

}

const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_flush_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_flush);

}

const dnxc_data_table_info_t *
dnx_data_l2_age_and_flush_machine_age_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_age_and_flush_machine, dnx_data_l2_age_and_flush_machine_table_age);

}






static shr_error_e
dnx_data_l2_olp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "olp";
    submodule_data->doc = "Data for OLP configuration";
    
    submodule_data->nof_features = _dnx_data_l2_olp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data l2 olp features");

    submodule_data->features[dnx_data_l2_olp_refresh_events_support].name = "refresh_events_support";
    submodule_data->features[dnx_data_l2_olp_refresh_events_support].doc = "Indicate if refresh events are supported";
    submodule_data->features[dnx_data_l2_olp_refresh_events_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].name = "olp_learn_payload_initial_value_supported";
    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].doc = "Indicate if olp learn payload initial value can be read from the register";
    submodule_data->features[dnx_data_l2_olp_olp_learn_payload_initial_value_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].name = "dsp_messages_support";
    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].doc = "Indicate if OLP can create DSP messages";
    submodule_data->features[dnx_data_l2_olp_dsp_messages_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_l2_olp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data l2 olp defines");

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].name = "lpkgv_shift";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].doc = "Learn_Payload_Key_Gen_Var shift in learn payload msb";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_shift].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].name = "lpkgv_mask";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].doc = "Learn_Payload_Key_Gen_Var mask in learn payload msb";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].name = "lpkgv_with_outlif";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].doc = "Learn_Payload_Key_Gen_Var for outlif is valid in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_with_outlif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].name = "lpkgv_wo_outlif";
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].doc = "Learn_Payload_Key_Gen_Var for outlif is invalid in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_lpkgv_wo_outlif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].name = "destination_offset";
    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].doc = "destination offset in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_destination_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].name = "outlif_offset";
    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].doc = "outlif offset in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_outlif_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].name = "eei_offset";
    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].doc = "eei offset in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_eei_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].name = "fec_offset";
    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].doc = "fec offset in learn payload";
    
    submodule_data->defines[dnx_data_l2_olp_define_fec_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].name = "jr_mode_enhanced_performance_enable";
    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].doc = "Enhance performance in jr mode";
    
    submodule_data->defines[dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_l2_olp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data l2 olp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_l2_olp_feature_get(
    int unit,
    dnx_data_l2_olp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, feature);
}


uint32
dnx_data_l2_olp_lpkgv_shift_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_shift);
}

uint32
dnx_data_l2_olp_lpkgv_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_mask);
}

uint32
dnx_data_l2_olp_lpkgv_with_outlif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_with_outlif);
}

uint32
dnx_data_l2_olp_lpkgv_wo_outlif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_lpkgv_wo_outlif);
}

uint32
dnx_data_l2_olp_destination_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_destination_offset);
}

uint32
dnx_data_l2_olp_outlif_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_outlif_offset);
}

uint32
dnx_data_l2_olp_eei_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_eei_offset);
}

uint32
dnx_data_l2_olp_fec_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_fec_offset);
}

uint32
dnx_data_l2_olp_jr_mode_enhanced_performance_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_l2, dnx_data_l2_submodule_olp, dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable);
}







shr_error_e
dnx_data_l2_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "l2";
    module_data->nof_submodules = _dnx_data_l2_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data l2 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_l2_general_init(unit, &module_data->submodules[dnx_data_l2_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_l2_feature_init(unit, &module_data->submodules[dnx_data_l2_submodule_feature]));
    SHR_IF_ERR_EXIT(dnx_data_l2_vsi_init(unit, &module_data->submodules[dnx_data_l2_submodule_vsi]));
    SHR_IF_ERR_EXIT(dnx_data_l2_vlan_domain_init(unit, &module_data->submodules[dnx_data_l2_submodule_vlan_domain]));
    SHR_IF_ERR_EXIT(dnx_data_l2_dma_init(unit, &module_data->submodules[dnx_data_l2_submodule_dma]));
    SHR_IF_ERR_EXIT(dnx_data_l2_age_and_flush_machine_init(unit, &module_data->submodules[dnx_data_l2_submodule_age_and_flush_machine]));
    SHR_IF_ERR_EXIT(dnx_data_l2_olp_init(unit, &module_data->submodules[dnx_data_l2_submodule_olp]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_l2_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_l2_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_l2_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

