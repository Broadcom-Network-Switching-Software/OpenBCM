
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>




extern shr_error_e jr2_a0_data_tunnel_attach(
    int unit);




static shr_error_e
dnx_data_tunnel_udp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "udp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tunnel_udp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel udp features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_udp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel udp defines");

    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].name = "nof_udp_ports_profiles";
    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_udp_define_first_udp_ports_profile].name = "first_udp_ports_profile";
    submodule_data->defines[dnx_data_tunnel_udp_define_first_udp_ports_profile].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_udp_define_first_udp_ports_profile].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_udp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel udp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_udp_feature_get(
    int unit,
    dnx_data_tunnel_udp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_udp, feature);
}


uint32
dnx_data_tunnel_udp_nof_udp_ports_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_udp, dnx_data_tunnel_udp_define_nof_udp_ports_profiles);
}

uint32
dnx_data_tunnel_udp_first_udp_ports_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_udp, dnx_data_tunnel_udp_define_first_udp_ports_profile);
}










static shr_error_e
dnx_data_tunnel_tunnel_type_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tunnel_type";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tunnel_tunnel_type_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel tunnel_type features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_tunnel_type_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel tunnel_type defines");

    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].name = "udp_tunnel_type";
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_gre_tunnel_type].name = "gre_tunnel_type";
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_gre_tunnel_type].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_gre_tunnel_type].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported].name = "geneve_tunnel_type_supported";
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_tunnel_type_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel tunnel_type tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_tunnel_type_feature_get(
    int unit,
    dnx_data_tunnel_tunnel_type_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tunnel_type, feature);
}


uint32
dnx_data_tunnel_tunnel_type_udp_tunnel_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tunnel_type, dnx_data_tunnel_tunnel_type_define_udp_tunnel_type);
}

uint32
dnx_data_tunnel_tunnel_type_gre_tunnel_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tunnel_type, dnx_data_tunnel_tunnel_type_define_gre_tunnel_type);
}

uint32
dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tunnel_type, dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported);
}










static shr_error_e
dnx_data_tunnel_roo_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "roo";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tunnel_roo_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel roo features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_roo_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel roo defines");

    submodule_data->defines[dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf].name = "terminate_only_8B_in_ip_at_term_block_using_pmf";
    submodule_data->defines[dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_roo_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel roo tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_roo_feature_get(
    int unit,
    dnx_data_tunnel_roo_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_roo, feature);
}


uint32
dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_roo, dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf);
}










static shr_error_e
dnx_data_tunnel_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tunnel_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel feature features");

    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported].name = "ipvx_tunnel_over_mpls_supported";
    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported].doc = "";
    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam].name = "ipvx_tunnel_over_mpls_use_vtt_tcam";
    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam].doc = "";
    submodule_data->features[dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_tunnel_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel feature defines");

    submodule_data->defines[dnx_data_tunnel_feature_define_tunnel_route_disable].name = "tunnel_route_disable";
    submodule_data->defines[dnx_data_tunnel_feature_define_tunnel_route_disable].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_tunnel_route_disable].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table].name = "dedicated_fodo_to_vni_table";
    submodule_data->defines[dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field].name = "dip_idx_type_is_lif_field";
    submodule_data->defines[dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api].name = "fodo_to_vni_entry_created_by_vlan_api";
    submodule_data->defines[dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type].name = "has_flow_gport_to_tunnel_type";
    submodule_data->defines[dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec].name = "support_learn_mode_async_lif_and_fec";
    submodule_data->defines[dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_l3_fodo_result_type_supported].name = "l3_fodo_result_type_supported";
    submodule_data->defines[dnx_data_tunnel_feature_define_l3_fodo_result_type_supported].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_l3_fodo_result_type_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_udp_tunnel_nat].name = "udp_tunnel_nat";
    submodule_data->defines[dnx_data_tunnel_feature_define_udp_tunnel_nat].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_udp_tunnel_nat].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_feature_define_short_network_domain].name = "short_network_domain";
    submodule_data->defines[dnx_data_tunnel_feature_define_short_network_domain].doc = "";
    
    submodule_data->defines[dnx_data_tunnel_feature_define_short_network_domain].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_feature_feature_get(
    int unit,
    dnx_data_tunnel_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, feature);
}


uint32
dnx_data_tunnel_feature_tunnel_route_disable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_tunnel_route_disable);
}

uint32
dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table);
}

uint32
dnx_data_tunnel_feature_dip_idx_type_is_lif_field_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field);
}

uint32
dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api);
}

uint32
dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type);
}

uint32
dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec);
}

uint32
dnx_data_tunnel_feature_l3_fodo_result_type_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_l3_fodo_result_type_supported);
}

uint32
dnx_data_tunnel_feature_udp_tunnel_nat_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_udp_tunnel_nat);
}

uint32
dnx_data_tunnel_feature_short_network_domain_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_feature, dnx_data_tunnel_feature_define_short_network_domain);
}







shr_error_e
dnx_data_tunnel_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "tunnel";
    module_data->nof_submodules = _dnx_data_tunnel_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data tunnel submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_tunnel_udp_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_udp]));
    SHR_IF_ERR_EXIT(dnx_data_tunnel_tunnel_type_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_tunnel_type]));
    SHR_IF_ERR_EXIT(dnx_data_tunnel_roo_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_roo]));
    SHR_IF_ERR_EXIT(dnx_data_tunnel_feature_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_feature]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

