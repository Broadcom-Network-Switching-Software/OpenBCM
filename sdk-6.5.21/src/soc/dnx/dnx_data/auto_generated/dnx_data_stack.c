

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STK

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stack.h>



extern shr_error_e jr2_a0_data_stack_attach(
    int unit);
extern shr_error_e j2c_a0_data_stack_attach(
    int unit);
extern shr_error_e q2a_a0_data_stack_attach(
    int unit);
extern shr_error_e j2p_a0_data_stack_attach(
    int unit);



static shr_error_e
dnx_data_stack_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "stack module general attributes";
    
    submodule_data->nof_features = _dnx_data_stack_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stack general features");

    
    submodule_data->nof_defines = _dnx_data_stack_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stack general defines");

    submodule_data->defines[dnx_data_stack_general_define_nof_tm_domains_max].name = "nof_tm_domains_max";
    submodule_data->defines[dnx_data_stack_general_define_nof_tm_domains_max].doc = "Max number of TM domains in the device";
    
    submodule_data->defines[dnx_data_stack_general_define_nof_tm_domains_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_min].name = "dest_port_base_queue_min";
    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_min].doc = "Min base queue value for stacking destination system port";
    
    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_max].name = "dest_port_base_queue_max";
    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_max].doc = "Max base queue value for stacking destination system port";
    
    submodule_data->defines[dnx_data_stack_general_define_dest_port_base_queue_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min].name = "lag_stack_trunk_resolve_entry_min";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min].doc = "Min entry for stack trunk resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max].name = "lag_stack_trunk_resolve_entry_max";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max].doc = "Max entry for stack trunk resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all].name = "lag_stack_trunk_resolve_entry_all";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all].doc = "All entries for stack trunk resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min].name = "lag_stack_fec_resolve_entry_min";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min].doc = "Min entry for stack fec resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max].name = "lag_stack_fec_resolve_entry_max";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max].doc = "Max entry for stack fec resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all].name = "lag_stack_fec_resolve_entry_all";
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all].doc = "All entries for stack fec resolve on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_domain_min].name = "lag_domain_min";
    submodule_data->defines[dnx_data_stack_general_define_lag_domain_min].doc = "Min value for stack lag tm-domain";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_domain_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_lag_domain_max].name = "lag_domain_max";
    submodule_data->defines[dnx_data_stack_general_define_lag_domain_max].doc = "Max value for stack lag tm-domain";
    
    submodule_data->defines[dnx_data_stack_general_define_lag_domain_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num].name = "trunk_entry_per_tmd_bit_num";
    submodule_data->defines[dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num].doc = "trunk entry per tmd bit number on stack lag";
    
    submodule_data->defines[dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stack_general_define_stacking_enable].name = "stacking_enable";
    submodule_data->defines[dnx_data_stack_general_define_stacking_enable].doc = "is stacking enabled on this device";
    submodule_data->defines[dnx_data_stack_general_define_stacking_enable].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stack_general_define_stacking_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_stack_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stack general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stack_general_feature_get(
    int unit,
    dnx_data_stack_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, feature);
}


uint32
dnx_data_stack_general_nof_tm_domains_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_nof_tm_domains_max);
}

uint32
dnx_data_stack_general_dest_port_base_queue_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_dest_port_base_queue_min);
}

uint32
dnx_data_stack_general_dest_port_base_queue_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_dest_port_base_queue_max);
}

uint32
dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min);
}

uint32
dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max);
}

uint32
dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all);
}

uint32
dnx_data_stack_general_lag_stack_fec_resolve_entry_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min);
}

uint32
dnx_data_stack_general_lag_stack_fec_resolve_entry_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max);
}

uint32
dnx_data_stack_general_lag_stack_fec_resolve_entry_all_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all);
}

uint32
dnx_data_stack_general_lag_domain_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_domain_min);
}

uint32
dnx_data_stack_general_lag_domain_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_lag_domain_max);
}

uint32
dnx_data_stack_general_trunk_entry_per_tmd_bit_num_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num);
}

uint32
dnx_data_stack_general_stacking_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stack, dnx_data_stack_submodule_general, dnx_data_stack_general_define_stacking_enable);
}







shr_error_e
dnx_data_stack_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "stack";
    module_data->nof_submodules = _dnx_data_stack_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data stack submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_stack_general_init(unit, &module_data->submodules[dnx_data_stack_submodule_general]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stack_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stack_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_stack_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

