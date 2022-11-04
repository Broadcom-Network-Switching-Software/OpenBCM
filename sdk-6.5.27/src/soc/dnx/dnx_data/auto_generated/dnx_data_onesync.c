
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KNETSYNC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_onesync.h>




extern shr_error_e j2c_a0_data_onesync_attach(
    int unit);


extern shr_error_e q2a_a0_data_onesync_attach(
    int unit);




static shr_error_e
dnx_data_onesync_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_onesync_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data onesync general features");

    
    submodule_data->nof_defines = _dnx_data_onesync_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data onesync general defines");

    
    submodule_data->nof_tables = _dnx_data_onesync_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data onesync general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_onesync_general_feature_get(
    int unit,
    dnx_data_onesync_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_onesync, dnx_data_onesync_submodule_general, feature);
}











static shr_error_e
dnx_data_onesync_onesync_firmware_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "onesync_firmware";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_onesync_onesync_firmware_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data onesync onesync_firmware features");

    
    submodule_data->nof_defines = _dnx_data_onesync_onesync_firmware_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data onesync onesync_firmware defines");

    submodule_data->defines[dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port].name = "knetsync_cosq_port";
    submodule_data->defines[dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port].doc = "";
    
    submodule_data->defines[dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_onesync_onesync_firmware_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data onesync onesync_firmware tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_onesync_onesync_firmware_feature_get(
    int unit,
    dnx_data_onesync_onesync_firmware_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_onesync, dnx_data_onesync_submodule_onesync_firmware, feature);
}


uint32
dnx_data_onesync_onesync_firmware_knetsync_cosq_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_onesync, dnx_data_onesync_submodule_onesync_firmware, dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port);
}







shr_error_e
dnx_data_onesync_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "onesync";
    module_data->nof_submodules = _dnx_data_onesync_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data onesync submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_onesync_general_init(unit, &module_data->submodules[dnx_data_onesync_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_onesync_onesync_firmware_init(unit, &module_data->submodules[dnx_data_onesync_submodule_onesync_firmware]));
    

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(j2c_a0_data_onesync_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(j2c_a0_data_onesync_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(q2a_a0_data_onesync_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(q2a_a0_data_onesync_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(q2a_a0_data_onesync_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

