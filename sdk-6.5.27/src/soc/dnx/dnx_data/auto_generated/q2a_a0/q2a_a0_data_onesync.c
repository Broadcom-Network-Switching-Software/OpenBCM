
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













static shr_error_e
q2a_a0_dnx_data_onesync_onesync_firmware_knetsync_cosq_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_onesync;
    int submodule_index = dnx_data_onesync_submodule_onesync_firmware;
    int define_index = dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 204;

    
    define->data = 204;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "knetsync_cosq_port";
    define->property.range_min = 1;
    define->property.range_max = 65536;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_onesync_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_onesync;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_onesync_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_onesync_submodule_onesync_firmware;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_onesync_onesync_firmware_define_knetsync_cosq_port;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_onesync_onesync_firmware_knetsync_cosq_port_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

