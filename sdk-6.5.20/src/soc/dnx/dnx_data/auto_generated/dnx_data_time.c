

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TIME

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_time.h>



extern shr_error_e jr2_a0_data_time_attach(
    int unit);



static shr_error_e
dnx_data_time_timesync_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "timesync";
    submodule_data->doc = "timesync attributes";
    
    submodule_data->nof_features = _dnx_data_time_timesync_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data time timesync features");

    submodule_data->features[dnx_data_time_timesync_open_loop_tsfifo_support].name = "open_loop_tsfifo_support";
    submodule_data->features[dnx_data_time_timesync_open_loop_tsfifo_support].doc = "tsfifo support for open loop tests";
    submodule_data->features[dnx_data_time_timesync_open_loop_tsfifo_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_time_timesync_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data time timesync defines");

    
    submodule_data->nof_tables = _dnx_data_time_timesync_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data time timesync tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_time_timesync_feature_get(
    int unit,
    dnx_data_time_timesync_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_time, dnx_data_time_submodule_timesync, feature);
}








shr_error_e
dnx_data_time_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "time";
    module_data->nof_submodules = _dnx_data_time_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data time submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_time_timesync_init(unit, &module_data->submodules[dnx_data_time_submodule_timesync]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_time_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

