

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>








static shr_error_e
q2a_a0_dnx_data_elk_general_flp_full_treshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_flp_full_treshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_elk_general_lookup_timeout_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_lookup_timeout_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7d0;

    
    define->data = 0x7d0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_elk_general_trans_timer_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_trans_timer_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10;

    
    define->data = 0x10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}












static shr_error_e
q2a_a0_dnx_data_elk_connectivity_connect_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int define_index = dnx_data_elk_connectivity_define_connect_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_TCAM_CONNECT_MODE;
    define->property.doc = 
        "\n"
        "External Tcam connection mode.\n"
        "SINGLE: One host connects with one KBP;\n"
        "DUAL_SHARED: Two hosts connect with one KBP via one thread;\n"
        "DUAL_SMT: Two hosts connect with one KBP via two threads.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SINGLE";
    define->property.mapping[0].val = 0;
    define->property.mapping[1].name = "SINGLE_HOST";
    define->property.mapping[1].val = 0;
    define->property.mapping[2].name = "DUAL_SHARED";
    define->property.mapping[2].val = 1;
    define->property.mapping[3].name = "DUAL_SMT";
    define->property.mapping[3].val = 2;
    define->property.mapping[4].name = "SINGLE_PORT";
    define->property.mapping[4].val = 3;
    define->property.mapping[4].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}









shr_error_e
q2a_a0_data_elk_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_elk;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_elk_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_general_define_flp_full_treshold;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_elk_general_flp_full_treshold_set;
    data_index = dnx_data_elk_general_define_lookup_timeout_delay;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_elk_general_lookup_timeout_delay_set;
    data_index = dnx_data_elk_general_define_trans_timer_delay;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_elk_general_trans_timer_delay_set;

    

    
    
    submodule_index = dnx_data_elk_submodule_application;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_elk_submodule_connectivity;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_connectivity_define_connect_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_elk_connectivity_connect_mode_set;

    

    
    
    submodule_index = dnx_data_elk_submodule_recovery;
    submodule = &module->submodules[submodule_index];

    

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

