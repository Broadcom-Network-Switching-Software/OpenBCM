

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_latency.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_latency_features_valid_flow_profile_flag_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_features;
    int feature_index = dnx_data_latency_features_valid_flow_profile_flag;
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
jr2_a0_dnx_data_latency_features_valid_end_to_end_aqm_profile_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_features;
    int feature_index = dnx_data_latency_features_valid_end_to_end_aqm_profile;
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
jr2_a0_dnx_data_latency_features_probabilistic_mechanism_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_features;
    int feature_index = dnx_data_latency_features_probabilistic_mechanism_support;
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
jr2_a0_dnx_data_latency_features_latency_measurement_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_features;
    int feature_index = dnx_data_latency_features_latency_measurement_support;
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
jr2_a0_dnx_data_latency_features_valid_track_expansion_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_features;
    int feature_index = dnx_data_latency_features_valid_track_expansion;
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
jr2_a0_dnx_data_latency_profile_egress_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_profile;
    int define_index = dnx_data_latency_profile_define_egress_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_latency_profile_ingress_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_profile;
    int define_index = dnx_data_latency_profile_define_ingress_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_latency_aqm_profiles_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_aqm;
    int define_index = dnx_data_latency_aqm_define_profiles_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_latency_aqm_flows_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_aqm;
    int define_index = dnx_data_latency_aqm_define_flows_nof;
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
jr2_a0_dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_latency;
    int submodule_index = dnx_data_latency_submodule_ingress;
    int define_index = dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_latency_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_latency;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_latency_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_latency_features_valid_flow_profile_flag;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_latency_features_valid_flow_profile_flag_set;
    data_index = dnx_data_latency_features_valid_end_to_end_aqm_profile;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_latency_features_valid_end_to_end_aqm_profile_set;
    data_index = dnx_data_latency_features_probabilistic_mechanism_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_latency_features_probabilistic_mechanism_support_set;
    data_index = dnx_data_latency_features_latency_measurement_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_latency_features_latency_measurement_support_set;
    data_index = dnx_data_latency_features_valid_track_expansion;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_latency_features_valid_track_expansion_set;

    
    
    submodule_index = dnx_data_latency_submodule_profile;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_latency_profile_define_egress_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_latency_profile_egress_nof_set;
    data_index = dnx_data_latency_profile_define_ingress_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_latency_profile_ingress_nof_set;

    

    
    
    submodule_index = dnx_data_latency_submodule_aqm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_latency_aqm_define_profiles_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_latency_aqm_profiles_nof_set;
    data_index = dnx_data_latency_aqm_define_flows_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_latency_aqm_flows_nof_set;

    

    
    
    submodule_index = dnx_data_latency_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_latency_ingress_define_jr_mode_latency_timestamp_left_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_latency_ingress_jr_mode_latency_timestamp_left_shift_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

