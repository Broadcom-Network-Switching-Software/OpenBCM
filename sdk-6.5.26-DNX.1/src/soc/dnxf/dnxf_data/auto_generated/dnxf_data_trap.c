
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_trap.h>



#ifdef BCM_DNXF1_SUPPORT

extern shr_error_e ramon_a0_data_trap_attach(
    int unit);

#endif 



static shr_error_e
dnxf_data_trap_etpp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "etpp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnxf_data_trap_etpp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data trap etpp features");

    submodule_data->features[dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support].name = "mtu_layer_to_compressed_layer_mapping_support";
    submodule_data->features[dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support].doc = "";
    submodule_data->features[dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_trap_etpp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data trap etpp defines");

    
    submodule_data->nof_tables = _dnxf_data_trap_etpp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data trap etpp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_trap_etpp_feature_get(
    int unit,
    dnxf_data_trap_etpp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_trap, dnxf_data_trap_submodule_etpp, feature);
}








shr_error_e
dnxf_data_trap_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "trap";
    module_data->nof_submodules = _dnxf_data_trap_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data trap submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_trap_etpp_init(unit, &module_data->submodules[dnxf_data_trap_submodule_etpp]));
    
#ifdef BCM_DNXF1_SUPPORT

    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_trap_attach(unit));
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

