
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_vlan.h>




extern shr_error_e jr2_a0_data_vlan_attach(
    int unit);


extern shr_error_e j2c_a0_data_vlan_attach(
    int unit);


extern shr_error_e q2a_a0_data_vlan_attach(
    int unit);


extern shr_error_e j2p_a0_data_vlan_attach(
    int unit);


extern shr_error_e j2x_a0_data_vlan_attach(
    int unit);




static shr_error_e
dnx_data_vlan_vlan_translate_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vlan_translate";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_vlan_vlan_translate_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data vlan vlan_translate features");

    
    submodule_data->nof_defines = _dnx_data_vlan_vlan_translate_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data vlan vlan_translate defines");

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats].name = "nof_vlan_tag_formats";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles].name = "nof_ingress_vlan_edit_profiles";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles].name = "nof_egress_vlan_edit_profiles";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries].name = "nof_vlan_range_entries";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry].name = "nof_vlan_ranges_per_entry";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index].name = "egress_tpid_outer_packet_index";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index].name = "egress_tpid_inner_packet_index";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids].name = "nof_ingress_vlan_edit_action_ids";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids].name = "nof_egress_vlan_edit_action_ids";
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids].doc = "";
    
    submodule_data->defines[dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_vlan_vlan_translate_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data vlan vlan_translate tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_vlan_vlan_translate_feature_get(
    int unit,
    dnx_data_vlan_vlan_translate_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, feature);
}


uint32
dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats);
}

uint32
dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles);
}

uint32
dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles);
}

uint32
dnx_data_vlan_vlan_translate_nof_vlan_range_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries);
}

uint32
dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry);
}

uint32
dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index);
}

uint32
dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index);
}

uint32
dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids);
}

uint32
dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_vlan_translate, dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids);
}










static shr_error_e
dnx_data_vlan_llvp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "llvp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_vlan_llvp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data vlan llvp features");

    submodule_data->features[dnx_data_vlan_llvp_packet_pcp_dei_support].name = "packet_pcp_dei_support";
    submodule_data->features[dnx_data_vlan_llvp_packet_pcp_dei_support].doc = "";
    submodule_data->features[dnx_data_vlan_llvp_packet_pcp_dei_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_vlan_llvp_outlif_llvp_from_outlif_profile].name = "outlif_llvp_from_outlif_profile";
    submodule_data->features[dnx_data_vlan_llvp_outlif_llvp_from_outlif_profile].doc = "";
    submodule_data->features[dnx_data_vlan_llvp_outlif_llvp_from_outlif_profile].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_vlan_llvp_eve_llvp_from_fhei_ext].name = "eve_llvp_from_fhei_ext";
    submodule_data->features[dnx_data_vlan_llvp_eve_llvp_from_fhei_ext].doc = "";
    submodule_data->features[dnx_data_vlan_llvp_eve_llvp_from_fhei_ext].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_vlan_llvp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data vlan llvp defines");

    submodule_data->defines[dnx_data_vlan_llvp_define_ext_vcp_en].name = "ext_vcp_en";
    submodule_data->defines[dnx_data_vlan_llvp_define_ext_vcp_en].doc = "";
    
    submodule_data->defines[dnx_data_vlan_llvp_define_ext_vcp_en].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_vlan_llvp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data vlan llvp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_vlan_llvp_feature_get(
    int unit,
    dnx_data_vlan_llvp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_llvp, feature);
}


uint32
dnx_data_vlan_llvp_ext_vcp_en_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_llvp, dnx_data_vlan_llvp_define_ext_vcp_en);
}










static shr_error_e
dnx_data_vlan_esem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "esem";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_vlan_esem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data vlan esem features");

    submodule_data->features[dnx_data_vlan_esem_ac_info].name = "ac_info";
    submodule_data->features[dnx_data_vlan_esem_ac_info].doc = "";
    submodule_data->features[dnx_data_vlan_esem_ac_info].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_vlan_esem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data vlan esem defines");

    
    submodule_data->nof_tables = _dnx_data_vlan_esem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data vlan esem tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_vlan_esem_feature_get(
    int unit,
    dnx_data_vlan_esem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_vlan, dnx_data_vlan_submodule_esem, feature);
}








shr_error_e
dnx_data_vlan_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "vlan";
    module_data->nof_submodules = _dnx_data_vlan_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data vlan submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_vlan_vlan_translate_init(unit, &module_data->submodules[dnx_data_vlan_submodule_vlan_translate]));
    SHR_IF_ERR_EXIT(dnx_data_vlan_llvp_init(unit, &module_data->submodules[dnx_data_vlan_submodule_llvp]));
    SHR_IF_ERR_EXIT(dnx_data_vlan_esem_init(unit, &module_data->submodules[dnx_data_vlan_submodule_esem]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_vlan_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_vlan_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_vlan_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

