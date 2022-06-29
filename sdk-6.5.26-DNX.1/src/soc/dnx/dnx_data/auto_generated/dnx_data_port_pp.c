
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT_PP

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port_pp.h>




extern shr_error_e jr2_a0_data_port_pp_attach(
    int unit);


extern shr_error_e j2c_a0_data_port_pp_attach(
    int unit);


extern shr_error_e j2c_a1_data_port_pp_attach(
    int unit);


extern shr_error_e q2a_a0_data_port_pp_attach(
    int unit);


extern shr_error_e j2p_a0_data_port_pp_attach(
    int unit);


extern shr_error_e j2x_a0_data_port_pp_attach(
    int unit);




static shr_error_e
dnx_data_port_pp_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_pp_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port_pp general features");

    submodule_data->features[dnx_data_port_pp_general_split_horizon_per_port_outlif_profile_support].name = "split_horizon_per_port_outlif_profile_support";
    submodule_data->features[dnx_data_port_pp_general_split_horizon_per_port_outlif_profile_support].doc = "";
    submodule_data->features[dnx_data_port_pp_general_split_horizon_per_port_outlif_profile_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_general_prt_tcam_is_direct_access].name = "prt_tcam_is_direct_access";
    submodule_data->features[dnx_data_port_pp_general_prt_tcam_is_direct_access].doc = "";
    submodule_data->features[dnx_data_port_pp_general_prt_tcam_is_direct_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_general_recycle_app_sbfd_reflector].name = "recycle_app_sbfd_reflector";
    submodule_data->features[dnx_data_port_pp_general_recycle_app_sbfd_reflector].doc = "";
    submodule_data->features[dnx_data_port_pp_general_recycle_app_sbfd_reflector].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_port_pp_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port_pp general defines");

    submodule_data->defines[dnx_data_port_pp_general_define_nof_ingress_llvp_profiles].name = "nof_ingress_llvp_profiles";
    submodule_data->defines[dnx_data_port_pp_general_define_nof_ingress_llvp_profiles].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_nof_ingress_llvp_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_llvp_profiles].name = "nof_egress_llvp_profiles";
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_llvp_profiles].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_llvp_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_default_ingress_native_ac_llvp_profile_id].name = "default_ingress_native_ac_llvp_profile_id";
    submodule_data->defines[dnx_data_port_pp_general_define_default_ingress_native_ac_llvp_profile_id].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_default_ingress_native_ac_llvp_profile_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_native_ac_llvp_profile_id].name = "default_egress_native_ac_llvp_profile_id";
    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_native_ac_llvp_profile_id].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_native_ac_llvp_profile_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_ip_tunnel_llvp_profile_id].name = "default_egress_ip_tunnel_llvp_profile_id";
    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_ip_tunnel_llvp_profile_id].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_default_egress_ip_tunnel_llvp_profile_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_sit_profile].name = "nof_egress_sit_profile";
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_sit_profile].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_sit_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_acceptable_frame_type_profile].name = "nof_egress_acceptable_frame_type_profile";
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_acceptable_frame_type_profile].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_nof_egress_acceptable_frame_type_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_sit_inner_eth_additional_tag].name = "sit_inner_eth_additional_tag";
    submodule_data->defines[dnx_data_port_pp_general_define_sit_inner_eth_additional_tag].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_sit_inner_eth_additional_tag].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_nof_virtual_port_tcam_entries].name = "nof_virtual_port_tcam_entries";
    submodule_data->defines[dnx_data_port_pp_general_define_nof_virtual_port_tcam_entries].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_nof_virtual_port_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_port_pp_general_define_rcy_info_prt_qualifier_eth_over_append64].name = "rcy_info_prt_qualifier_eth_over_append64";
    submodule_data->defines[dnx_data_port_pp_general_define_rcy_info_prt_qualifier_eth_over_append64].doc = "";
    
    submodule_data->defines[dnx_data_port_pp_general_define_rcy_info_prt_qualifier_eth_over_append64].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_port_pp_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port_pp general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_pp_general_feature_get(
    int unit,
    dnx_data_port_pp_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, feature);
}


uint32
dnx_data_port_pp_general_nof_ingress_llvp_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_nof_ingress_llvp_profiles);
}

uint32
dnx_data_port_pp_general_nof_egress_llvp_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_nof_egress_llvp_profiles);
}

uint32
dnx_data_port_pp_general_default_ingress_native_ac_llvp_profile_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_default_ingress_native_ac_llvp_profile_id);
}

uint32
dnx_data_port_pp_general_default_egress_native_ac_llvp_profile_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_default_egress_native_ac_llvp_profile_id);
}

uint32
dnx_data_port_pp_general_default_egress_ip_tunnel_llvp_profile_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_default_egress_ip_tunnel_llvp_profile_id);
}

uint32
dnx_data_port_pp_general_nof_egress_sit_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_nof_egress_sit_profile);
}

uint32
dnx_data_port_pp_general_nof_egress_acceptable_frame_type_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_nof_egress_acceptable_frame_type_profile);
}

uint32
dnx_data_port_pp_general_sit_inner_eth_additional_tag_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_sit_inner_eth_additional_tag);
}

uint32
dnx_data_port_pp_general_nof_virtual_port_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_nof_virtual_port_tcam_entries);
}

uint32
dnx_data_port_pp_general_rcy_info_prt_qualifier_eth_over_append64_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_general, dnx_data_port_pp_general_define_rcy_info_prt_qualifier_eth_over_append64);
}










static shr_error_e
dnx_data_port_pp_filters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "filters";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_port_pp_filters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data port_pp filters features");

    submodule_data->features[dnx_data_port_pp_filters_acceptable_frame_type_filter_support].name = "acceptable_frame_type_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_acceptable_frame_type_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_acceptable_frame_type_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_dss_stacking_filter_support].name = "dss_stacking_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_dss_stacking_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_dss_stacking_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_lag_egress_multicast_filter_support].name = "lag_egress_multicast_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_lag_egress_multicast_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_lag_egress_multicast_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_exclude_source_filter_support].name = "exclude_source_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_exclude_source_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_exclude_source_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_ipv6_hop_by_hop_filter_support].name = "ipv6_hop_by_hop_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_ipv6_hop_by_hop_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_ipv6_hop_by_hop_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_glem_filter_support].name = "glem_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_glem_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_glem_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_ppmc_not_found_filter_support].name = "ppmc_not_found_filter_support";
    submodule_data->features[dnx_data_port_pp_filters_ppmc_not_found_filter_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_ppmc_not_found_filter_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_rqp_discard_and_invalid_otm_filters_support].name = "rqp_discard_and_invalid_otm_filters_support";
    submodule_data->features[dnx_data_port_pp_filters_rqp_discard_and_invalid_otm_filters_support].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_rqp_discard_and_invalid_otm_filters_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_port_pp_filters_egress_filters_enable_per_recycle_port_supported].name = "egress_filters_enable_per_recycle_port_supported";
    submodule_data->features[dnx_data_port_pp_filters_egress_filters_enable_per_recycle_port_supported].doc = "";
    submodule_data->features[dnx_data_port_pp_filters_egress_filters_enable_per_recycle_port_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_port_pp_filters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data port_pp filters defines");

    
    submodule_data->nof_tables = _dnx_data_port_pp_filters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data port_pp filters tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_port_pp_filters_feature_get(
    int unit,
    dnx_data_port_pp_filters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_port_pp, dnx_data_port_pp_submodule_filters, feature);
}








shr_error_e
dnx_data_port_pp_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "port_pp";
    module_data->nof_submodules = _dnx_data_port_pp_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data port_pp submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_port_pp_general_init(unit, &module_data->submodules[dnx_data_port_pp_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_port_pp_filters_init(unit, &module_data->submodules[dnx_data_port_pp_submodule_filters]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_port_pp_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_port_pp_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_port_pp_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

