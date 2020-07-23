

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINKSCANDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_linkscan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_linkscan_general_error_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_linkscan;
    int submodule_index = dnx_data_linkscan_submodule_general;
    int define_index = dnx_data_linkscan_general_define_error_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000000;

    
    define->data = 10000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM_LINKSCAN_ERRDELAY;
    define->property.doc = 
        "\n"
        "The amount of time in microseconds for which the bcm_linkscan module will suspend\n"
        "a port from further update processing after 'bcm_linkscan_maxerr' errors are detected.\n"
        "After this delay, the error state for the port is cleared and normal linkscan processing\n"
        "resumes on the port\n"
        "bcm_linkscan_errdelay= 0~0xffffffff\n"
        "Default: 10,000,000\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_linkscan_general_max_error_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_linkscan;
    int submodule_index = dnx_data_linkscan_submodule_general;
    int define_index = dnx_data_linkscan_general_define_max_error;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM_LINKSCAN_MAXERR;
    define->property.doc = 
        "\n"
        "The number of port update errors which will cause the bcm_linkscan module to suspend\n"
        "a port from update processing for the period of time set in 'bcm_linkscan_errdelay'\n"
        "bcm_linkscan_maxerr= 0~0xffffffff\n"
        "Default: 5\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_linkscan_general_thread_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_linkscan;
    int submodule_index = dnx_data_linkscan_submodule_general;
    int define_index = dnx_data_linkscan_general_define_thread_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_LINKSCAN_THREAD_PRI;
    define->property.doc = 
        "\n"
        "Specifies the priority of the BCM Linkscan thread\n"
        "linkcan_thread_pri= 0~0xffffffff\n"
        "Default: 50\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 99;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_linkscan_general_m0_pause_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_linkscan;
    int submodule_index = dnx_data_linkscan_submodule_general;
    int define_index = dnx_data_linkscan_general_define_m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "custom_feature_linkscan_m0_pause='0' or '1'\n"
        "if 1 will pause firmware while linkscan thread process.\n"
        "Default - '0'\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "linkscan_m0_pause";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_linkscan_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_linkscan;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_linkscan_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_linkscan_general_define_error_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_linkscan_general_error_delay_set;
    data_index = dnx_data_linkscan_general_define_max_error;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_linkscan_general_max_error_set;
    data_index = dnx_data_linkscan_general_define_thread_priority;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_linkscan_general_thread_priority_set;
    data_index = dnx_data_linkscan_general_define_m0_pause_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_linkscan_general_m0_pause_enable_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

