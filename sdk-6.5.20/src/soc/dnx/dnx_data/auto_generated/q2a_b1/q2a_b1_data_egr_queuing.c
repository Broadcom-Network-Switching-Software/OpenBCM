

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm/port.h>
#include <soc/mcm/allenum.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>








static shr_error_e
q2a_b1_dnx_data_egr_queuing_params_burst_size_below_mtu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = dnx_data_egr_queuing_submodule_params;
    int define_index = dnx_data_egr_queuing_params_define_burst_size_below_mtu;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "indicated whether burst size can be configured bellow MTU\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "burst_size_below_mtu";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_b1_data_egr_queuing_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_egr_queuing;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_egr_queuing_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_egr_queuing_params_define_burst_size_below_mtu;
    define = &submodule->defines[data_index];
    define->set = q2a_b1_dnx_data_egr_queuing_params_burst_size_below_mtu_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

