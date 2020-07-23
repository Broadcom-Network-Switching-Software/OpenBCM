

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TIME

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_time_timesync_open_loop_tsfifo_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_time;
    int submodule_index = dnx_data_time_submodule_timesync;
    int feature_index = dnx_data_time_timesync_open_loop_tsfifo_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = "tsfifo_open_loop";
    feature->property.doc = 
        "\n"
        "tsfifo_open_loop=0/1\n"
        "internal usage only: Default 0\n"
        "the soc prop should be enabled only for open loop tests,\n"
        "this soc prop should not be enabled otherwise\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_time_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_time;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_time_submodule_timesync;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_time_timesync_open_loop_tsfifo_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_time_timesync_open_loop_tsfifo_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

