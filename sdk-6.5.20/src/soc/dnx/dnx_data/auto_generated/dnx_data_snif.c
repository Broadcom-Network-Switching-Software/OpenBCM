

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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



static shr_error_e
dnx_data_snif_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "SNIF general data";
    
    submodule_data->nof_features = _dnx_data_snif_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif general features");

    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].name = "original_destination_is_supported";
    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].doc = "is destination OW can be omited";
    submodule_data->features[dnx_data_snif_general_original_destination_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_snif_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif general defines");

    submodule_data->defines[dnx_data_snif_general_define_cud_type].name = "cud_type";
    submodule_data->defines[dnx_data_snif_general_define_cud_type].doc = "Sniff CUD type";
    
    submodule_data->defines[dnx_data_snif_general_define_cud_type].flags |= DNXC_DATA_F_DEFINE;

    
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
dnx_data_snif_general_cud_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_general, dnx_data_snif_general_define_cud_type);
}










static shr_error_e
dnx_data_snif_ingress_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ingress";
    submodule_data->doc = "SNIF ingress profiles data";
    
    submodule_data->nof_features = _dnx_data_snif_ingress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif ingress features");

    
    submodule_data->nof_defines = _dnx_data_snif_ingress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif ingress defines");

    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].doc = "Number of SNIF profiles supported by ingress SNIF";
    
    submodule_data->defines[dnx_data_snif_ingress_define_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].name = "mirror_nof_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].doc = "Number of Mirror/Snoop profiles supported by ingress SNIF";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].name = "nof_mirror_on_drop_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].doc = "Number of Mirror on drop profiles supported";
    
    submodule_data->defines[dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].name = "mirror_nof_vlan_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].doc = "Number of mirror vlan profiles supported by ingress SNIF";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_nof_vlan_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].name = "mirror_default_tagged_profile";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].doc = "The ingress default vlan profile for tagged packets";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_default_tagged_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].name = "mirror_untagged_profiles";
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].doc = "The ingress vlan profile for untagged packets";
    
    submodule_data->defines[dnx_data_snif_ingress_define_mirror_untagged_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].name = "prob_max_val";
    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].doc = "Probability max value";
    
    submodule_data->defines[dnx_data_snif_ingress_define_prob_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].name = "crop_size";
    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].doc = "crop size";
    
    submodule_data->defines[dnx_data_snif_ingress_define_crop_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext].name = "keep_orig_dest_on_dsp_ext";
    submodule_data->defines[dnx_data_snif_ingress_define_keep_orig_dest_on_dsp_ext].doc = "stamping destination on FTM dsp extension";
    
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
    submodule_data->doc = "SNIF egress profiles data";
    
    submodule_data->nof_features = _dnx_data_snif_egress_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data snif egress features");

    submodule_data->features[dnx_data_snif_egress_probability_sample].name = "probability_sample";
    submodule_data->features[dnx_data_snif_egress_probability_sample].doc = "egress mirror supports probability sampling";
    submodule_data->features[dnx_data_snif_egress_probability_sample].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_snif_egress_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data snif egress defines");

    submodule_data->defines[dnx_data_snif_egress_define_mirror_snoop_nof_profiles].name = "mirror_snoop_nof_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_snoop_nof_profiles].doc = "Number of Mirror/Snoop profiles supported by egress SNIF";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_snoop_nof_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].name = "mirror_nof_vlan_profiles";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].doc = "Number of mirror vlan profiles supported by egress SNIF";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_nof_vlan_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].name = "mirror_default_profile";
    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].doc = "The egress default vlan profile for tagged packets";
    
    submodule_data->defines[dnx_data_snif_egress_define_mirror_default_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].name = "prob_max_val";
    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].doc = "Probability max value";
    
    submodule_data->defines[dnx_data_snif_egress_define_prob_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_snif_egress_define_crop_size].name = "crop_size";
    submodule_data->defines[dnx_data_snif_egress_define_crop_size].doc = "crop size";
    
    submodule_data->defines[dnx_data_snif_egress_define_crop_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_snif_egress_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data snif egress tables");


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
dnx_data_snif_egress_mirror_snoop_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_mirror_snoop_nof_profiles);
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
dnx_data_snif_egress_crop_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_snif, dnx_data_snif_submodule_egress, dnx_data_snif_egress_define_crop_size);
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

