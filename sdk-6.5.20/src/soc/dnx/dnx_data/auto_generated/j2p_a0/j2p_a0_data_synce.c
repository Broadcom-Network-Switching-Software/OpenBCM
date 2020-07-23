

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_synce.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_synce_general_synce_no_sdm_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int feature_index = dnx_data_synce_general_synce_no_sdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_synce_general_synce_nmg_chain_select_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int feature_index = dnx_data_synce_general_synce_nmg_chain_select;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_synce_general_fabric_div_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int define_index = dnx_data_synce_general_define_fabric_div_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_synce_general_fabric_div_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int define_index = dnx_data_synce_general_define_fabric_div_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_synce_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_synce;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_synce_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_synce_general_define_fabric_div_min;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_synce_general_fabric_div_min_set;
    data_index = dnx_data_synce_general_define_fabric_div_max;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_synce_general_fabric_div_max_set;

    
    data_index = dnx_data_synce_general_synce_no_sdm_mode;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_synce_general_synce_no_sdm_mode_set;
    data_index = dnx_data_synce_general_synce_nmg_chain_select;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_synce_general_synce_nmg_chain_select_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

