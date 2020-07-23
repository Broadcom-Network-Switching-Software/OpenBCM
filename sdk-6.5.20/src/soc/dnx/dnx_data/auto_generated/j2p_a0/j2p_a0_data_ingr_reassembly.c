

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_context;
    int feature_index = dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
j2p_a0_dnx_data_ingr_reassembly_dbal_interface_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_interface_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_ingr_reassembly_dbal_interface_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_interface_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 71;

    
    define->data = 71;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    int define_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit) + 4;

    
    define->data = dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit) + 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_ingr_reassembly_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ingr_reassembly;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ingr_reassembly_submodule_context;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities_set;

    
    
    submodule_index = dnx_data_ingr_reassembly_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingr_reassembly_dbal_define_interface_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ingr_reassembly_dbal_interface_bits_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_interface_max;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ingr_reassembly_dbal_interface_max_set;
    data_index = dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

