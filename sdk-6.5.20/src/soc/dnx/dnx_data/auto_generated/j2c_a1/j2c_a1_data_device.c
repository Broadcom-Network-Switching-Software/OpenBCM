

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
j2c_a1_dnx_data_device_general_core_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_device;
    int submodule_index = dnx_data_device_submodule_general;
    int define_index = dnx_data_device_general_define_core_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000000;

    
    define->data = 1000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CORE_CLOCK_SPEED;
    define->property.doc = 
        "\n"
        "Device core clock frequency in KHZ\n"
        "Synopsis: core_clock_speed_khz=#freq#\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "khz";
    define->property.range_min = 300000;
    define->property.range_max = 3000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a1_dnx_data_device_general_device_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_device;
    int submodule_index = dnx_data_device_submodule_general;
    int define_index = dnx_data_device_general_define_device_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x8800;

    
    define->data = 0x8800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a1_data_device_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_device;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_device_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_device_general_define_core_clock_khz;
    define = &submodule->defines[data_index];
    define->set = j2c_a1_dnx_data_device_general_core_clock_khz_set;
    data_index = dnx_data_device_general_define_device_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a1_dnx_data_device_general_device_id_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

