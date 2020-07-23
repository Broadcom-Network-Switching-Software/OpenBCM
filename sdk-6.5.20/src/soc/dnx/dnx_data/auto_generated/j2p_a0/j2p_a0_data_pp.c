

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT_PP

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_pp_PEM_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_PEM;
    int feature_index = dnx_data_pp_PEM_clock_power_down;
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
j2p_a0_dnx_data_pp_PEM_nof_pem_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_PEM;
    int define_index = dnx_data_pp_PEM_define_nof_pem_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_pp_ETPP_etps_type_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_ETPP;
    int define_index = dnx_data_pp_ETPP_define_etps_type_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_pp_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_pp;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_pp_submodule_PEM;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_PEM_define_nof_pem_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pp_PEM_nof_pem_bits_set;

    
    data_index = dnx_data_pp_PEM_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_pp_PEM_clock_power_down_set;

    
    
    submodule_index = dnx_data_pp_submodule_ETPP;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_ETPP_define_etps_type_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_pp_ETPP_etps_type_size_bits_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

