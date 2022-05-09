
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_snif.h>




extern shr_error_e jr2_a0_data_snif_attach(
    int unit);


extern shr_error_e j2c_a0_data_snif_attach(
    int unit);


extern shr_error_e q2a_a0_data_snif_attach(
    int unit);


extern shr_error_e j2p_a0_data_snif_attach(
    int unit);


extern shr_error_e j2p_a2_data_snif_attach(
    int unit);




static shr_error_e
dnx_data_snif_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_snif_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif general features");

    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].name = "original_destination_is_supported";
    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].doc = "";
    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_snif_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif general defines");

    submodule_data->defines[dnx_data_snif_general_define_cud_type_standard].name = "cud_type_standard";
    submodule_data->defines[dnx_data_snif_general_define_cud_type_standard].doc = "";
    
    submodule_data->defines[dnx_data_snif_general_define_cud_type_standard].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_general_define_cud_type_sflow].name = "cud_type_sflow";
    submodule_data->defines[dnx_data_snif_general_define_cud_type_sflow].doc = "";
    
    submodule_data->defines[dnx_data_snif_general_define_cud_type_sflow].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_general_define_cud_type_mirror_on_drop].name = "cud_type_mirror_on_drop";
    submodule_data->defines[dnx_data_snif_general_define_cud_type_mirror_on_drop].doc = "";
    
    submodule_data->defines[dnx_data_snif_general_define_cud_type_mirror_on_drop].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_snif_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data snif general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_snif_general_feature_get(
    int unit,
    dnx_data_snif_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_general, feature);
}


uint32
dnx_data_snif_general_cud_type_standard_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_general, dnx_data_snif_general_define_cud_type_standard);
}

uint32
dnx_data_snif_general_cud_type_sflow_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_general, dnx_data_snif_general_define_cud_type_sflow);
}

uint32
dnx_data_snif_general_cud_type_mirror_on_drop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_general, dnx_data_snif_general_define_cud_type_mirror_on_drop);
}










static shr_error_e
dnx_data_snif_ingress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ingress";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_snif_ingress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif ingress features");

    submodule_data->features[dnx_data_snif_ingress_crop_size_config_is_supported].name = "crop_size_config_is_supported";
    submodule_data->features[dnx_data_snif_ingress_crop_size_config_is_supported].doc = "";
    submodule_data->features[dnx_data_snif_ingress_crop_size_config_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_ingress_mirror_on_drop_admit_profile_supported].name = "mirror_on_drop_admit_profile_supported";
    submodule_data->features[dnx_data_snif_ingress_mirror_on_drop_admit_profile_supported].doc = "";
    submodule_data->features[dnx_data_snif_ingress_mirror_on_drop_admit_profile_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_ingress_ext_stat_dedicated_config].name = "ext_stat_dedicated_config";
    submodule_data->features[dnx_data_snif_ingress_ext_stat_dedicated_config].doc = "";
    submodule_data->features[dnx_data_snif_ingress_ext_stat_dedicated_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_ingress_itpp_delta_supported].name = "itpp_delta_supported";
    submodule_data->features[dnx_data_snif_ingress_itpp_delta_supported].doc = "";
    submodule_data->features[dnx_data_snif_ingress_itpp_delta_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_snif_ingress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif ingress defines");

    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].name = "mirror_nof_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].name = "nof_mirror_on_drop_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].name = "mirror_nof_vlan_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].name = "mirror_default_tagged_profile";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].name = "mirror_untagged_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].name = "prob_max_val";
    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].name = "crop_size";
    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext].name = "keep_orig_dest_on_dsp_ext";
    submodule_data->defines[dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext].doc = "";
    
    submodule_data->defines[dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_snif_ingress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data snif ingress tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_snif_ingress_feature_get(
    int unit,
    dnx_data_snif_ingress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, feature);
}


uint32
dnx_data_snif_ingress_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_nof_profiles);
}

uint32
dnx_data_snif_ingress_mirror_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_mirror_nof_profiles);
}

uint32
dnx_data_snif_ingress_nof_mirror_on_drop_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles);
}

uint32
dnx_data_snif_ingress_mirror_nof_vlan_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_mirror_nof_vlan_profiles);
}

uint32
dnx_data_snif_ingress_mirror_default_tagged_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_mirror_default_tagged_profile);
}

uint32
dnx_data_snif_ingress_mirror_untagged_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_mirror_untagged_profiles);
}

uint32
dnx_data_snif_ingress_prob_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_prob_max_val);
}

uint32
dnx_data_snif_ingress_crop_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_crop_size);
}

uint32
dnx_data_snif_ingress_keep_orig_dest_on_dsp_ext_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_ingress, dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext);
}










static shr_error_e
dnx_data_snif_egress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "egress";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_snif_egress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif egress features");

    submodule_data->features[dnx_data_snif_egress_probability_sample].name = "probability_sample";
    submodule_data->features[dnx_data_snif_egress_probability_sample].doc = "";
    submodule_data->features[dnx_data_snif_egress_probability_sample].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_egress_egress_null_profile].name = "egress_null_profile";
    submodule_data->features[dnx_data_snif_egress_egress_null_profile].doc = "";
    submodule_data->features[dnx_data_snif_egress_egress_null_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_egress_probability_profiles].name = "probability_profiles";
    submodule_data->features[dnx_data_snif_egress_probability_profiles].doc = "";
    submodule_data->features[dnx_data_snif_egress_probability_profiles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_egress_crop_size_profiles].name = "crop_size_profiles";
    submodule_data->features[dnx_data_snif_egress_crop_size_profiles].doc = "";
    submodule_data->features[dnx_data_snif_egress_crop_size_profiles].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_snif_egress_egress_mirror_mode].name = "egress_mirror_mode";
    submodule_data->features[dnx_data_snif_egress_egress_mirror_mode].doc = "";
    submodule_data->features[dnx_data_snif_egress_egress_mirror_mode].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_snif_egress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif egress defines");

    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_profiles].name = "mirror_nof_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_snoop_nof_profiles].name = "snoop_nof_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_snoop_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_snoop_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].name = "mirror_nof_vlan_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].name = "mirror_default_profile";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].name = "prob_max_val";
    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_prob_nof_bits].name = "prob_nof_bits";
    submodule_data->defines[dnx_data_snif_egress_define_prob_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_prob_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_probability_nof_profiles].name = "probability_nof_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_probability_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_probability_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_crop_size_nof_profiles].name = "crop_size_nof_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_crop_size_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_crop_size_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_crop_size_granularity].name = "crop_size_granularity";
    submodule_data->defines[dnx_data_snif_egress_define_crop_size_granularity].doc = "";
    
    submodule_data->defines[dnx_data_snif_egress_define_crop_size_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_snif_egress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data snif egress tables");

    
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].name = "rcy_mirror_to_forward_port_map";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].doc = "";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].size_of_values = sizeof(dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t);
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].entry_get = dnx_data_snif_egress_rcy_mirror_to_forward_port_map_entry_str_get;

    
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].nof_keys = 1;
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].keys[0].name = "port";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].nof_values, "_dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map table values");
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].values[0].name = "forward_port";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].values[0].type = "int";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].values[0].doc = "forward port to map";
    submodule_data->tables[dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t, forward_port);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_snif_egress_feature_get(
    int unit,
    dnx_data_snif_egress_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, feature);
}


uint32
dnx_data_snif_egress_mirror_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_mirror_nof_profiles);
}

uint32
dnx_data_snif_egress_snoop_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_snoop_nof_profiles);
}

uint32
dnx_data_snif_egress_mirror_nof_vlan_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_mirror_nof_vlan_profiles);
}

uint32
dnx_data_snif_egress_mirror_default_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_mirror_default_profile);
}

uint32
dnx_data_snif_egress_prob_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_prob_max_val);
}

uint32
dnx_data_snif_egress_prob_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_prob_nof_bits);
}

uint32
dnx_data_snif_egress_probability_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_probability_nof_profiles);
}

uint32
dnx_data_snif_egress_crop_size_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_crop_size_nof_profiles);
}

uint32
dnx_data_snif_egress_crop_size_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_crop_size_granularity);
}



const dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t *
dnx_data_snif_egress_rcy_mirror_to_forward_port_map_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t *) data;

}


shr_error_e
dnx_data_snif_egress_rcy_mirror_to_forward_port_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map);
    data = (const dnx_data_snif_egress_rcy_mirror_to_forward_port_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->forward_port);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_snif_egress_rcy_mirror_to_forward_port_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_table_rcy_mirror_to_forward_port_map);

}



shr_error_e
dnx_data_snif_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "snif";
    module_data->nof_submodules = _dnx_data_snif_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data snif submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_snif_general_init(unit, &module_data->submodules[dnx_data_snif_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_snif_ingress_init(unit, &module_data->submodules[dnx_data_snif_submodule_ingress]));
    SHR_IF_ERR_EXIT(dnx_data_snif_egress_init(unit, &module_data->submodules[dnx_data_snif_submodule_egress]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_snif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a2_data_snif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_snif_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

