
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_flow_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_flow_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_flow_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general info";
    
    submodule_data->nof_features = _dnx_data_flow_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow general features");

    submodule_data->features[dnx_data_flow_general_is_tests_supported].name = "is_tests_supported";
    submodule_data->features[dnx_data_flow_general_is_tests_supported].doc = "";
    submodule_data->features[dnx_data_flow_general_is_tests_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_general_is_virtual_supported].name = "is_virtual_supported";
    submodule_data->features[dnx_data_flow_general_is_virtual_supported].doc = "indicates whether virtual flag is supported for this device";
    submodule_data->features[dnx_data_flow_general_is_virtual_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_general_is_lif_profile_dedicated_logic_enabled].name = "is_lif_profile_dedicated_logic_enabled";
    submodule_data->features[dnx_data_flow_general_is_lif_profile_dedicated_logic_enabled].doc = "Flow Module is used to determine if ingress or egress lif_profile is set via dedicated logic or not";
    submodule_data->features[dnx_data_flow_general_is_lif_profile_dedicated_logic_enabled].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_general_is_flow_enabled_for_legacy_applications].name = "is_flow_enabled_for_legacy_applications";
    submodule_data->features[dnx_data_flow_general_is_flow_enabled_for_legacy_applications].doc = "indicates whether certain flow features are relevant for legacy applications";
    submodule_data->features[dnx_data_flow_general_is_flow_enabled_for_legacy_applications].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow general defines");

    
    submodule_data->nof_tables = _dnx_data_flow_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_general_feature_get(
    int unit,
    dnx_data_flow_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_general, feature);
}











static shr_error_e
dnx_data_flow_ipv4_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipv4";
    submodule_data->doc = "IPV4 tunnel data";
    
    submodule_data->nof_features = _dnx_data_flow_ipv4_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow ipv4 features");

    
    submodule_data->nof_defines = _dnx_data_flow_ipv4_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow ipv4 defines");

    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].name = "use_flow_lif_init";
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].doc = "Use flow lif to configure init tunnels";
    
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].name = "use_flow_lif_term";
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].doc = "Use flow lif to configure term tunnels";
    
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_flow_ipv4_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow ipv4 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_ipv4_feature_get(
    int unit,
    dnx_data_flow_ipv4_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, feature);
}


uint32
dnx_data_flow_ipv4_use_flow_lif_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, dnx_data_flow_ipv4_define_use_flow_lif_init);
}

uint32
dnx_data_flow_ipv4_use_flow_lif_term_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, dnx_data_flow_ipv4_define_use_flow_lif_term);
}










static shr_error_e
dnx_data_flow_twamp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "twamp";
    submodule_data->doc = "TWAMP";
    
    submodule_data->nof_features = _dnx_data_flow_twamp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow twamp features");

    
    submodule_data->nof_defines = _dnx_data_flow_twamp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow twamp defines");

    submodule_data->defines[dnx_data_flow_twamp_define_twamp_reflector_supported].name = "twamp_reflector_supported";
    submodule_data->defines[dnx_data_flow_twamp_define_twamp_reflector_supported].doc = "Use twamp reflector flow application";
    
    submodule_data->defines[dnx_data_flow_twamp_define_twamp_reflector_supported].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_flow_twamp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow twamp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_twamp_feature_get(
    int unit,
    dnx_data_flow_twamp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_twamp, feature);
}


uint32
dnx_data_flow_twamp_twamp_reflector_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_twamp, dnx_data_flow_twamp_define_twamp_reflector_supported);
}







shr_error_e
dnx_data_flow_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "flow";
    module_data->nof_submodules = _dnx_data_flow_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data flow submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_flow_general_init(unit, &module_data->submodules[dnx_data_flow_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_flow_ipv4_init(unit, &module_data->submodules[dnx_data_flow_submodule_ipv4]));
    SHR_IF_ERR_EXIT(dnx_data_flow_twamp_init(unit, &module_data->submodules[dnx_data_flow_submodule_twamp]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
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

