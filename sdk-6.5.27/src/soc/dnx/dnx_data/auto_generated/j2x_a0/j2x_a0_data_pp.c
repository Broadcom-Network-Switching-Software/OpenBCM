
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT_PP

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pp.h>







static shr_error_e
j2x_a0_dnx_data_pp_PEM_clock_power_down_set(
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
j2x_a0_dnx_data_pp_PEM_nof_pem_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_PEM;
    int define_index = dnx_data_pp_PEM_define_nof_pem_bits;
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
j2x_a0_dnx_data_pp_application_vtt1_lookup_by_exem1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_application;
    int define_index = dnx_data_pp_application_define_vtt1_lookup_by_exem1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_VLAN_LOOKUP_PHYSICAL_DATABASE_MODE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_pp_ETPP_etps_type_size_bits_set(
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


static shr_error_e
j2x_a0_dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_ETPP;
    int define_index = dnx_data_pp_ETPP_define_btc_max_header_to_add_in_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_pp_debug_ptc_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_ptc_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_pp_debug_valid_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_valid_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_pp_debug_time_stamp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_time_stamp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 34;

    
    define->data = 34;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_pp_attach(
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
    define->set = j2x_a0_dnx_data_pp_PEM_nof_pem_bits_set;

    
    data_index = dnx_data_pp_PEM_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_pp_PEM_clock_power_down_set;

    
    
    submodule_index = dnx_data_pp_submodule_application;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_application_define_vtt1_lookup_by_exem1;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_application_vtt1_lookup_by_exem1_set;

    

    
    
    submodule_index = dnx_data_pp_submodule_ETPP;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_ETPP_define_etps_type_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_ETPP_etps_type_size_bits_set;
    data_index = dnx_data_pp_ETPP_define_btc_max_header_to_add_in_bytes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_ETPP_btc_max_header_to_add_in_bytes_set;

    

    
    
    submodule_index = dnx_data_pp_submodule_debug;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_debug_define_ptc_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_debug_ptc_size_set;
    data_index = dnx_data_pp_debug_define_valid_bytes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_debug_valid_bytes_set;
    data_index = dnx_data_pp_debug_define_time_stamp;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_pp_debug_time_stamp_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

