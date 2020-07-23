

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINKSCANDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_linkscan.h>



extern shr_error_e jr2_a0_data_linkscan_attach(
    int unit);



static shr_error_e
dnx_data_linkscan_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general data";
    
    submodule_data->nof_features = _dnx_data_linkscan_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data linkscan general features");

    
    submodule_data->nof_defines = _dnx_data_linkscan_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data linkscan general defines");

    submodule_data->defines[dnx_data_linkscan_general_define_error_delay].name = "error_delay";
    submodule_data->defines[dnx_data_linkscan_general_define_error_delay].doc = "the amount of time in microseconds for which the bcm_linkscan module will suspend a port from further update processing after 'max_error' errors are detected.  After this delay, the error state for the port is cleared and normal linkscan processing resumes on the port";
    
    submodule_data->defines[dnx_data_linkscan_general_define_error_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_linkscan_general_define_max_error].name = "max_error";
    submodule_data->defines[dnx_data_linkscan_general_define_max_error].doc = "The number of port update errors which will cause the bcm_linkscan module to suspend a port from update processing for the period of time set in 'error_delay'";
    
    submodule_data->defines[dnx_data_linkscan_general_define_max_error].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_linkscan_general_define_thread_priority].name = "thread_priority";
    submodule_data->defines[dnx_data_linkscan_general_define_thread_priority].doc = "specifies the priority of the BCM Linkscan thread";
    
    submodule_data->defines[dnx_data_linkscan_general_define_thread_priority].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_linkscan_general_define_m0_pause_enable].name = "m0_pause_enable";
    submodule_data->defines[dnx_data_linkscan_general_define_m0_pause_enable].doc = "specifies the pause m0 firmware at Linkscan thread";
    
    submodule_data->defines[dnx_data_linkscan_general_define_m0_pause_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_linkscan_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data linkscan general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_linkscan_general_feature_get(
    int unit,
    dnx_data_linkscan_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_linkscan, dnx_data_linkscan_submodule_general, feature);
}


uint32
dnx_data_linkscan_general_error_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_linkscan, dnx_data_linkscan_submodule_general, dnx_data_linkscan_general_define_error_delay);
}

uint32
dnx_data_linkscan_general_max_error_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_linkscan, dnx_data_linkscan_submodule_general, dnx_data_linkscan_general_define_max_error);
}

uint32
dnx_data_linkscan_general_thread_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_linkscan, dnx_data_linkscan_submodule_general, dnx_data_linkscan_general_define_thread_priority);
}

uint32
dnx_data_linkscan_general_m0_pause_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_linkscan, dnx_data_linkscan_submodule_general, dnx_data_linkscan_general_define_m0_pause_enable);
}







shr_error_e
dnx_data_linkscan_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "linkscan";
    module_data->nof_submodules = _dnx_data_linkscan_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data linkscan submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_linkscan_general_init(unit, &module_data->submodules[dnx_data_linkscan_submodule_general]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_linkscan_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

