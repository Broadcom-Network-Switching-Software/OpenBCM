

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_multicast.h>



extern shr_error_e jr2_a0_data_multicast_attach(
    int unit);
extern shr_error_e q2a_a0_data_multicast_attach(
    int unit);



static shr_error_e
dnx_data_multicast_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "values for different multicast groups";
    
    submodule_data->nof_features = _dnx_data_multicast_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data multicast params features");

    
    submodule_data->nof_defines = _dnx_data_multicast_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data multicast params defines");

    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].name = "max_ing_mc_groups";
    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].doc = "maximum ingress multicast groups id";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_ing_mc_groups].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].name = "max_egr_mc_groups";
    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].doc = "maximum egress multicast groups id";
    
    submodule_data->defines[dnx_data_multicast_params_define_max_egr_mc_groups].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].name = "nof_ing_mc_bitmaps";
    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].doc = "number of multicast bitmap groups ids";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_ing_mc_bitmaps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].name = "nof_egr_mc_bitmaps";
    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].doc = "number of multicast bitmap groups ids";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_egr_mc_bitmaps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].name = "mcdb_allocation_method";
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].doc = "The method of mcdb allocation";
    
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_allocation_method].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].name = "nof_mcdb_entries_bits";
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].doc = "number of multicast entries in bits";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].name = "mcdb_block_size";
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].doc = "number of multicast entries in block";
    
    submodule_data->defines[dnx_data_multicast_params_define_mcdb_block_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].name = "nof_mcdb_entries";
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].doc = "number of multicast entries";
    
    submodule_data->defines[dnx_data_multicast_params_define_nof_mcdb_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_multicast_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data multicast params tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_multicast_params_feature_get(
    int unit,
    dnx_data_multicast_params_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, feature);
}


int
dnx_data_multicast_params_max_ing_mc_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_ing_mc_groups);
}

int
dnx_data_multicast_params_max_egr_mc_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_max_egr_mc_groups);
}

uint32
dnx_data_multicast_params_nof_ing_mc_bitmaps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_ing_mc_bitmaps);
}

uint32
dnx_data_multicast_params_nof_egr_mc_bitmaps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_egr_mc_bitmaps);
}

uint32
dnx_data_multicast_params_mcdb_allocation_method_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_mcdb_allocation_method);
}

uint32
dnx_data_multicast_params_nof_mcdb_entries_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_mcdb_entries_bits);
}

uint32
dnx_data_multicast_params_mcdb_block_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_mcdb_block_size);
}

uint32
dnx_data_multicast_params_nof_mcdb_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_multicast, dnx_data_multicast_submodule_params, dnx_data_multicast_params_define_nof_mcdb_entries);
}







shr_error_e
dnx_data_multicast_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "multicast";
    module_data->nof_submodules = _dnx_data_multicast_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data multicast submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_multicast_params_init(unit, &module_data->submodules[dnx_data_multicast_submodule_params]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_multicast_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_multicast_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

