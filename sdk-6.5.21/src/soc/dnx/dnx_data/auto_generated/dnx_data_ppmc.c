

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PPMC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ppmc.h>



extern shr_error_e jr2_a0_data_ppmc_attach(
    int unit);



static shr_error_e
dnx_data_ppmc_param_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "param";
    submodule_data->doc = "default values for the PPMC table actions";
    
    submodule_data->nof_features = _dnx_data_ppmc_param_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ppmc param features");

    
    submodule_data->nof_defines = _dnx_data_ppmc_param_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ppmc param defines");

    submodule_data->defines[dnx_data_ppmc_param_define_max_mc_replication_id].name = "max_mc_replication_id";
    submodule_data->defines[dnx_data_ppmc_param_define_max_mc_replication_id].doc = "Maximum value of multicast replication in the PPMC table";
    
    submodule_data->defines[dnx_data_ppmc_param_define_max_mc_replication_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ppmc_param_define_min_mc_replication_id].name = "min_mc_replication_id";
    submodule_data->defines[dnx_data_ppmc_param_define_min_mc_replication_id].doc = "Minimum value of multicast replication in the PPMC table";
    
    submodule_data->defines[dnx_data_ppmc_param_define_min_mc_replication_id].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ppmc_param_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ppmc param tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ppmc_param_feature_get(
    int unit,
    dnx_data_ppmc_param_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ppmc, dnx_data_ppmc_submodule_param, feature);
}


uint32
dnx_data_ppmc_param_max_mc_replication_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ppmc, dnx_data_ppmc_submodule_param, dnx_data_ppmc_param_define_max_mc_replication_id);
}

uint32
dnx_data_ppmc_param_min_mc_replication_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ppmc, dnx_data_ppmc_submodule_param, dnx_data_ppmc_param_define_min_mc_replication_id);
}







shr_error_e
dnx_data_ppmc_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ppmc";
    module_data->nof_submodules = _dnx_data_ppmc_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ppmc submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ppmc_param_init(unit, &module_data->submodules[dnx_data_ppmc_submodule_param]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ppmc_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

