

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_a0_dnx_data_ipq_queues_voq_resolution_per_dp_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int feature_index = dnx_data_ipq_queues_voq_resolution_per_dp;
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
q2a_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int feature_index = dnx_data_ipq_queues_tc_profile_from_tm_command;
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
q2a_a0_dnx_data_ipq_queues_nof_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_nof_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32*1024;

    
    define->data = 32*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_ipq_queues_queue_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_queue_id_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_ipq_queues_voq_offset_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_voq_offset_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
q2a_a0_dnx_data_ipq_regions_nof_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_regions;
    int define_index = dnx_data_ipq_regions_define_nof_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
q2a_a0_dnx_data_ipq_tc_map_nof_voq_flow_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_nof_voq_flow_profiles;
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
q2a_a0_dnx_data_ipq_tc_map_voq_flow_profiles_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_voq_flow_profiles_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_ipq_tc_map_voq_resolution_type_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_voq_resolution_type_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_ipq_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ipq;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ipq_submodule_queues;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ipq_queues_define_nof_queues;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_queues_nof_queues_set;
    data_index = dnx_data_ipq_queues_define_queue_id_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_queues_queue_id_bits_set;
    data_index = dnx_data_ipq_queues_define_voq_offset_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_queues_voq_offset_bits_set;
    data_index = dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_set;

    
    data_index = dnx_data_ipq_queues_voq_resolution_per_dp;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_ipq_queues_voq_resolution_per_dp_set;
    data_index = dnx_data_ipq_queues_tc_profile_from_tm_command;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_set;

    
    
    submodule_index = dnx_data_ipq_submodule_regions;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ipq_regions_define_nof_queues;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_regions_nof_queues_set;

    

    
    
    submodule_index = dnx_data_ipq_submodule_tc_map;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ipq_tc_map_define_nof_voq_flow_profiles;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_tc_map_nof_voq_flow_profiles_set;
    data_index = dnx_data_ipq_tc_map_define_voq_flow_profiles_bits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_tc_map_voq_flow_profiles_bits_set;
    data_index = dnx_data_ipq_tc_map_define_voq_resolution_type_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_ipq_tc_map_voq_resolution_type_max_value_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

