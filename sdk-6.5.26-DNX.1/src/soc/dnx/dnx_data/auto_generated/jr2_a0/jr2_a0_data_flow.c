
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>







static shr_error_e
jr2_a0_dnx_data_flow_general_is_tests_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_is_tests_supported;
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
jr2_a0_dnx_data_flow_general_is_inlif_legacy_profile_logic_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_is_inlif_legacy_profile_logic;
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
jr2_a0_dnx_data_flow_general_prp_profile_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_prp_profile_supported;
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
jr2_a0_dnx_data_flow_general_is_flow_enabled_for_legacy_applications_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_is_flow_enabled_for_legacy_applications;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "is_flow_enabled_for_legacy_applications";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_flow_srv6_use_flow_lif_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_srv6;
    int define_index = dnx_data_flow_srv6_define_use_flow_lif_init;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_init";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_srv6_use_flow_lif_term_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_srv6;
    int define_index = dnx_data_flow_srv6_define_use_flow_lif_term;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_term";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_twamp_twamp_reflector_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_twamp;
    int define_index = dnx_data_flow_twamp_define_twamp_reflector_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_flow_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_flow;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_flow_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_general_is_tests_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_is_tests_supported_set;
    data_index = dnx_data_flow_general_is_inlif_legacy_profile_logic;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_is_inlif_legacy_profile_logic_set;
    data_index = dnx_data_flow_general_prp_profile_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_prp_profile_supported_set;
    data_index = dnx_data_flow_general_is_flow_enabled_for_legacy_applications;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_is_flow_enabled_for_legacy_applications_set;

    
    
    submodule_index = dnx_data_flow_submodule_srv6;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_flow_srv6_define_use_flow_lif_init;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_srv6_use_flow_lif_init_set;
    data_index = dnx_data_flow_srv6_define_use_flow_lif_term;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_srv6_use_flow_lif_term_set;

    

    
    
    submodule_index = dnx_data_flow_submodule_twamp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_flow_twamp_define_twamp_reflector_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_twamp_twamp_reflector_supported_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

