

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>



extern shr_error_e jr2_a0_data_tunnel_attach(
    int unit);



static shr_error_e
dnx_data_tunnel_ipv6_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipv6";
    submodule_data->doc = "IPV6 tunnel data";
    
    submodule_data->nof_features = _dnx_data_tunnel_ipv6_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel ipv6 features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_ipv6_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel ipv6 defines");

    submodule_data->defines[dnx_data_tunnel_ipv6_define_nof_terminated_sips].name = "nof_terminated_sips";
    submodule_data->defines[dnx_data_tunnel_ipv6_define_nof_terminated_sips].doc = "Number of ipv6 tunnel sip profile id";
    
    submodule_data->defines[dnx_data_tunnel_ipv6_define_nof_terminated_sips].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_ipv6_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel ipv6 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_ipv6_feature_get(
    int unit,
    dnx_data_tunnel_ipv6_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_ipv6, feature);
}


uint32
dnx_data_tunnel_ipv6_nof_terminated_sips_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_ipv6, dnx_data_tunnel_ipv6_define_nof_terminated_sips);
}










static shr_error_e
dnx_data_tunnel_udp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "udp";
    submodule_data->doc = "UDP tunnels data.";
    
    submodule_data->nof_features = _dnx_data_tunnel_udp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel udp features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_udp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel udp defines");

    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].name = "nof_udp_ports_profiles";
    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].doc = "Number of udp ports profiles in the additional header profiles table.";
    
    submodule_data->defines[dnx_data_tunnel_udp_define_nof_udp_ports_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tunnel_udp_define_first_udp_ports_profile].name = "first_udp_ports_profile";
    submodule_data->defines[dnx_data_tunnel_udp_define_first_udp_ports_profile].doc = "Lowest udp ports profiles in the additional header profiles table that's not the default profile.";
    
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







shr_error_e
dnx_data_tunnel_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "tunnel";
    module_data->nof_submodules = _dnx_data_tunnel_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data tunnel submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_tunnel_ipv6_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_ipv6]));
    SHR_IF_ERR_EXIT(dnx_data_tunnel_udp_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_udp]));
    
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
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

