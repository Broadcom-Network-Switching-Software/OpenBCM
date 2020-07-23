

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

#define DNX_DATA_INTERNAL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_multicast.h>
#include <bcm_int/dnx/algo/multicast/algo_multicast.h>









static shr_error_e
j2c_a0_dnx_data_multicast_params_mcdb_allocation_method_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_mcdb_allocation_method;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_ALGO_MULTICAST_ALLOCATION_BASIC;

    
    define->data = DNX_ALGO_MULTICAST_ALLOCATION_BASIC;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_multicast_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_multicast;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_multicast_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_multicast_params_define_mcdb_allocation_method;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_multicast_params_mcdb_allocation_method_set;

    

    

    SHR_FUNC_EXIT;
}

