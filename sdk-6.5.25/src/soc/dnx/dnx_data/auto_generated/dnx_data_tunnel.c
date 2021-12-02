
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_tunnel_attach(
    int unit);

#endif 



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










static shr_error_e
dnx_data_tunnel_tunnel_type_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tunnel_type";
    submodule_data->doc = "tunnel type";
    
    submodule_data->nof_features = _dnx_data_tunnel_tunnel_type_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel tunnel_type features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_tunnel_type_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel tunnel_type defines");

    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].name = "udp_tunnel_type";
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].doc = "udp tunnel type is used by udp p2p/mp tunnel term tables";
    
    submodule_data->defines[dnx_data_tunnel_tunnel_type_define_udp_tunnel_type].flags |= DNXC_DATA_F_DEFINE;

    
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










static shr_error_e
dnx_data_tunnel_tcam_table_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tcam_table";
    submodule_data->doc = "tcam table properties";
    
    submodule_data->nof_features = _dnx_data_tunnel_tcam_table_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tunnel tcam_table features");

    
    submodule_data->nof_defines = _dnx_data_tunnel_tcam_table_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tunnel tcam_table defines");

    submodule_data->defines[dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields].name = "has_raw_udp_tunnel_type_is_2nd_pass_key_fields";
    submodule_data->defines[dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields].doc = "indiate if the tcam table has is_raw, udp_tunnel_type and 2nd_pass key fields";
    
    submodule_data->defines[dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tunnel_tcam_table_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tunnel tcam_table tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tunnel_tcam_table_feature_get(
    int unit,
    dnx_data_tunnel_tcam_table_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tcam_table, feature);
}


uint32
dnx_data_tunnel_tcam_table_has_raw_udp_tunnel_type_is_2nd_pass_key_fields_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tunnel, dnx_data_tunnel_submodule_tcam_table, dnx_data_tunnel_tcam_table_define_has_raw_udp_tunnel_type_is_2nd_pass_key_fields);
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
    SHR_IF_ERR_EXIT(dnx_data_tunnel_tcam_table_init(unit, &module_data->submodules[dnx_data_tunnel_submodule_tcam_table]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tunnel_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

