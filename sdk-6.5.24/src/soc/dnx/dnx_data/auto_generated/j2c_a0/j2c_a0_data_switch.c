
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>








static shr_error_e
j2c_a0_dnx_data_switch_load_balancing_padded_layers_crc_buffer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_padded_layers_crc_buffer;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_switch_load_balancing_wide_hash_buffer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_wide_hash_buffer;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_switch_load_balancing_nof_lb_mpls_stack_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_lb_mpls_stack;
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
j2c_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
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
j2c_a0_dnx_data_switch_feature_silent_dummy_lif_lookup_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_silent_dummy_lif_lookup;
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
j2c_a0_data_switch_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_switch;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_switch_submodule_load_balancing;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_switch_load_balancing_define_padded_layers_crc_buffer;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_switch_load_balancing_padded_layers_crc_buffer_set;
    data_index = dnx_data_switch_load_balancing_define_wide_hash_buffer;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_switch_load_balancing_wide_hash_buffer_set;
    data_index = dnx_data_switch_load_balancing_define_nof_lb_mpls_stack;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_switch_load_balancing_nof_lb_mpls_stack_set;

    

    
    
    submodule_index = dnx_data_switch_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set;
    data_index = dnx_data_switch_feature_silent_dummy_lif_lookup;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_switch_feature_silent_dummy_lif_lookup_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

