

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>












static shr_error_e
j2p_a0_dnx_data_sat_rx_flow_param_natural_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_natural_id;
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
j2p_a0_dnx_data_sat_rx_flow_param_odd_even_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_odd_even_id;
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
j2p_a0_dnx_data_sat_rx_flow_param_twamp_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_twamp_mode;
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
j2p_a0_dnx_data_sat_rx_flow_param_twamp_rx_time_stamp_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sat;
    int submodule_index = dnx_data_sat_submodule_rx_flow_param;
    int feature_index = dnx_data_sat_rx_flow_param_twamp_rx_time_stamp;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
j2p_a0_data_sat_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_sat;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_sat_submodule_generator;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_sat_submodule_rx_flow_param;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_sat_rx_flow_param_natural_id;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sat_rx_flow_param_natural_id_set;
    data_index = dnx_data_sat_rx_flow_param_odd_even_id;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sat_rx_flow_param_odd_even_id_set;
    data_index = dnx_data_sat_rx_flow_param_twamp_mode;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sat_rx_flow_param_twamp_mode_set;
    data_index = dnx_data_sat_rx_flow_param_twamp_rx_time_stamp;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_sat_rx_flow_param_twamp_rx_time_stamp_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

