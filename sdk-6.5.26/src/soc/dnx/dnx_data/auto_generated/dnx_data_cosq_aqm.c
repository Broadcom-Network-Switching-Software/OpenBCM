
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_cosq_aqm.h>




extern shr_error_e jr2_a0_data_cosq_aqm_attach(
    int unit);


extern shr_error_e j2c_a0_data_cosq_aqm_attach(
    int unit);


extern shr_error_e q2a_a0_data_cosq_aqm_attach(
    int unit);


extern shr_error_e j2p_a0_data_cosq_aqm_attach(
    int unit);


extern shr_error_e j2x_a0_data_cosq_aqm_attach(
    int unit);




static shr_error_e
dnx_data_cosq_aqm_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_cosq_aqm_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data cosq_aqm general features");

    submodule_data->features[dnx_data_cosq_aqm_general_is_aqm_supported].name = "is_aqm_supported";
    submodule_data->features[dnx_data_cosq_aqm_general_is_aqm_supported].doc = "";
    submodule_data->features[dnx_data_cosq_aqm_general_is_aqm_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_cosq_aqm_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data cosq_aqm general defines");

    submodule_data->defines[dnx_data_cosq_aqm_general_define_profile_nof].name = "profile_nof";
    submodule_data->defines[dnx_data_cosq_aqm_general_define_profile_nof].doc = "";
    
    submodule_data->defines[dnx_data_cosq_aqm_general_define_profile_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_cosq_aqm_general_define_flows_nof].name = "flows_nof";
    submodule_data->defines[dnx_data_cosq_aqm_general_define_flows_nof].doc = "";
    
    submodule_data->defines[dnx_data_cosq_aqm_general_define_flows_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_cosq_aqm_general_define_reserved_flows_nof].name = "reserved_flows_nof";
    submodule_data->defines[dnx_data_cosq_aqm_general_define_reserved_flows_nof].doc = "";
    
    submodule_data->defines[dnx_data_cosq_aqm_general_define_reserved_flows_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_cosq_aqm_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data cosq_aqm general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_cosq_aqm_general_feature_get(
    int unit,
    dnx_data_cosq_aqm_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_cosq_aqm, dnx_data_cosq_aqm_submodule_general, feature);
}


uint32
dnx_data_cosq_aqm_general_profile_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_cosq_aqm, dnx_data_cosq_aqm_submodule_general, dnx_data_cosq_aqm_general_define_profile_nof);
}

uint32
dnx_data_cosq_aqm_general_flows_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_cosq_aqm, dnx_data_cosq_aqm_submodule_general, dnx_data_cosq_aqm_general_define_flows_nof);
}

uint32
dnx_data_cosq_aqm_general_reserved_flows_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_cosq_aqm, dnx_data_cosq_aqm_submodule_general, dnx_data_cosq_aqm_general_define_reserved_flows_nof);
}







shr_error_e
dnx_data_cosq_aqm_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "cosq_aqm";
    module_data->nof_submodules = _dnx_data_cosq_aqm_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data cosq_aqm submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_cosq_aqm_general_init(unit, &module_data->submodules[dnx_data_cosq_aqm_submodule_general]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_cosq_aqm_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_cosq_aqm_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_cosq_aqm_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

