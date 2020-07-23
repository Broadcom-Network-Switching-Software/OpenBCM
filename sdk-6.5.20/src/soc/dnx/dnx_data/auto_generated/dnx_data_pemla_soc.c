

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pemla_soc.h>



extern shr_error_e jr2_a0_data_pemla_soc_attach(
    int unit);
extern shr_error_e jr2_b0_data_pemla_soc_attach(
    int unit);
extern shr_error_e j2c_a0_data_pemla_soc_attach(
    int unit);
extern shr_error_e q2a_a0_data_pemla_soc_attach(
    int unit);
extern shr_error_e q2a_b0_data_pemla_soc_attach(
    int unit);
extern shr_error_e j2p_a0_data_pemla_soc_attach(
    int unit);



static shr_error_e
dnx_data_pemla_soc_soc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "soc";
    submodule_data->doc = "pemla soc properties";
    
    submodule_data->nof_features = _dnx_data_pemla_soc_soc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pemla_soc soc features");

    
    submodule_data->nof_defines = _dnx_data_pemla_soc_soc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pemla_soc soc defines");

    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR1_MODE].name = "JR1_MODE";
    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR1_MODE].doc = "Backward compatible with Jericho/Kalia";
    
    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR1_MODE].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR2_MODE].name = "JR2_MODE";
    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR2_MODE].doc = "Not JR1 backward compatible - JR2 only";
    
    submodule_data->defines[dnx_data_pemla_soc_soc_define_JR2_MODE].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pemla_soc_soc_define_system_headers_mode].name = "system_headers_mode";
    submodule_data->defines[dnx_data_pemla_soc_soc_define_system_headers_mode].doc = "JR1_MODE: backward compatible with Jericho/Kalia                                                      JR2_MODE: not backward compatible";
    
    submodule_data->defines[dnx_data_pemla_soc_soc_define_system_headers_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_pemla_soc_soc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pemla_soc soc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pemla_soc_soc_feature_get(
    int unit,
    dnx_data_pemla_soc_soc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pemla_soc, dnx_data_pemla_soc_submodule_soc, feature);
}


uint32
dnx_data_pemla_soc_soc_JR1_MODE_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pemla_soc, dnx_data_pemla_soc_submodule_soc, dnx_data_pemla_soc_soc_define_JR1_MODE);
}

uint32
dnx_data_pemla_soc_soc_JR2_MODE_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pemla_soc, dnx_data_pemla_soc_submodule_soc, dnx_data_pemla_soc_soc_define_JR2_MODE);
}

uint32
dnx_data_pemla_soc_soc_system_headers_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pemla_soc, dnx_data_pemla_soc_submodule_soc, dnx_data_pemla_soc_soc_define_system_headers_mode);
}







shr_error_e
dnx_data_pemla_soc_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "pemla_soc";
    module_data->nof_submodules = _dnx_data_pemla_soc_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data pemla_soc submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_pemla_soc_soc_init(unit, &module_data->submodules[dnx_data_pemla_soc_submodule_soc]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_pemla_soc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pemla_soc_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pemla_soc_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

