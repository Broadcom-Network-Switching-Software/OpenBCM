
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







static shr_error_e
ramon_a0_dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_trap;
    int submodule_index = dnxf_data_trap_submodule_etpp;
    int feature_index = dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
ramon_a0_data_trap_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_trap;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_trap_submodule_etpp;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

