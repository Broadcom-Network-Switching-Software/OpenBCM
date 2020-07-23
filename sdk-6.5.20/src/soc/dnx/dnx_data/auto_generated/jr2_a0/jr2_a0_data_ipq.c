

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
#include <shared/utilex/utilex_integer_arithmetic.h>








static shr_error_e
jr2_a0_dnx_data_ipq_queues_nof_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_nof_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64*1024;

    
    define->data = 64*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_queues_min_bundle_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_min_bundle_size;
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
jr2_a0_dnx_data_ipq_queues_fmq_bundle_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_fmq_bundle_size;
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
jr2_a0_dnx_data_ipq_queues_max_fmq_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_max_fmq_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16*1024-1;

    
    define->data = 16*1024-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_queues_queue_id_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_queue_id_bits;
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
jr2_a0_dnx_data_ipq_queues_voq_offset_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_voq_offset_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_queues_min_bundle_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_min_bundle_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_queues_flow_quartet_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_flow_quartet_nof_bits;
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
jr2_a0_dnx_data_ipq_queues_invalid_queue_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_queues;
    int define_index = dnx_data_ipq_queues_define_invalid_queue;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_power_of_2(dnx_data_ipq.queues.queue_id_bits_get(unit)) - 1;

    
    define->data = utilex_power_of_2(dnx_data_ipq.queues.queue_id_bits_get(unit)) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_ipq_regions_nof_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_regions;
    int define_index = dnx_data_ipq_regions_define_nof_queues;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_ipq_tc_map_nof_voq_flow_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_nof_voq_flow_profiles;
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
jr2_a0_dnx_data_ipq_tc_map_voq_flow_profiles_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_voq_flow_profiles_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_tc_map_default_voq_flow_profile_idx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_tc_map_nof_sysport_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_nof_sysport_profiles;
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
jr2_a0_dnx_data_ipq_tc_map_default_sysport_profile_idx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_default_sysport_profile_idx;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ipq_tc_map_voq_resolution_type_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ipq;
    int submodule_index = dnx_data_ipq_submodule_tc_map;
    int define_index = dnx_data_ipq_tc_map_define_voq_resolution_type_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_ipq_attach(
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
    define->set = jr2_a0_dnx_data_ipq_queues_nof_queues_set;
    data_index = dnx_data_ipq_queues_define_min_bundle_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_min_bundle_size_set;
    data_index = dnx_data_ipq_queues_define_fmq_bundle_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_fmq_bundle_size_set;
    data_index = dnx_data_ipq_queues_define_max_fmq_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_max_fmq_id_set;
    data_index = dnx_data_ipq_queues_define_queue_id_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_queue_id_bits_set;
    data_index = dnx_data_ipq_queues_define_voq_offset_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_voq_offset_bits_set;
    data_index = dnx_data_ipq_queues_define_min_bundle_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_min_bundle_size_bits_set;
    data_index = dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_set;
    data_index = dnx_data_ipq_queues_define_flow_quartet_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_flow_quartet_nof_bits_set;
    data_index = dnx_data_ipq_queues_define_invalid_queue;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_queues_invalid_queue_set;

    

    
    
    submodule_index = dnx_data_ipq_submodule_regions;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ipq_regions_define_nof_queues;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_regions_nof_queues_set;

    

    
    
    submodule_index = dnx_data_ipq_submodule_tc_map;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ipq_tc_map_define_nof_voq_flow_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_nof_voq_flow_profiles_set;
    data_index = dnx_data_ipq_tc_map_define_voq_flow_profiles_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_voq_flow_profiles_bits_set;
    data_index = dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_default_voq_flow_profile_idx_set;
    data_index = dnx_data_ipq_tc_map_define_nof_sysport_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_nof_sysport_profiles_set;
    data_index = dnx_data_ipq_tc_map_define_default_sysport_profile_idx;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_default_sysport_profile_idx_set;
    data_index = dnx_data_ipq_tc_map_define_voq_resolution_type_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ipq_tc_map_voq_resolution_type_max_value_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

