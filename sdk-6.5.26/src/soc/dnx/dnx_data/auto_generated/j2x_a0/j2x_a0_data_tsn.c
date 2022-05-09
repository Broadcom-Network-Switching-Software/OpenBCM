
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tsn.h>







static shr_error_e
j2x_a0_dnx_data_tsn_general_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_general;
    int feature_index = dnx_data_tsn_general_is_supported;
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
j2x_a0_dnx_data_tsn_preemption_verify_max_time_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_verify_max_time;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_preemption_verify_max_attempts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_verify_max_attempts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_preemption_non_final_frag_size_tx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_non_final_frag_size_tx;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_preemption_final_frag_size_tx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_final_frag_size_tx;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_preemption_non_final_frag_size_rx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_non_final_frag_size_rx;
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
j2x_a0_dnx_data_tsn_preemption_final_frag_size_rx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_final_frag_size_rx;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_preemption;
    int define_index = dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_tsn_time_tick_period_ns_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tick_period_ns;
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
j2x_a0_dnx_data_tsn_time_tick_period_min_ns_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tick_period_min_ns;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400;

    
    define->data = 400;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_time_tsn_counter_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tsn_counter_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_time_tas_tod_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tas_tod_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_time_tas_tod_source_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tas_tod_source;
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
j2x_a0_dnx_data_tsn_tas_max_time_intervals_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tas;
    int define_index = dnx_data_tsn_tas_define_max_time_intervals_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 127;

    
    define->data = 127;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_tas_nof_tas_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tas;
    int define_index = dnx_data_tsn_tas_define_nof_tas_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_ilkn_alloc_start_get(unit) + 1;

    
    define->data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_ilkn_alloc_start_get(unit) + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_tas_max_profiles_per_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tas;
    int define_index = dnx_data_tsn_tas_define_max_profiles_per_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_tsn_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tsn;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_tsn_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_tsn_general_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_tsn_general_is_supported_set;

    
    
    submodule_index = dnx_data_tsn_submodule_preemption;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_preemption_define_verify_max_time;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_verify_max_time_set;
    data_index = dnx_data_tsn_preemption_define_verify_max_attempts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_verify_max_attempts_set;
    data_index = dnx_data_tsn_preemption_define_non_final_frag_size_tx;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_non_final_frag_size_tx_set;
    data_index = dnx_data_tsn_preemption_define_final_frag_size_tx;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_final_frag_size_tx_set;
    data_index = dnx_data_tsn_preemption_define_non_final_frag_size_rx;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_non_final_frag_size_rx_set;
    data_index = dnx_data_tsn_preemption_define_final_frag_size_rx;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_final_frag_size_rx_set;
    data_index = dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_set;

    

    
    
    submodule_index = dnx_data_tsn_submodule_time;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_time_define_tick_period_ns;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tick_period_ns_set;
    data_index = dnx_data_tsn_time_define_tick_period_min_ns;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tick_period_min_ns_set;
    data_index = dnx_data_tsn_time_define_tsn_counter_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tsn_counter_nof_bits_set;
    data_index = dnx_data_tsn_time_define_tas_tod_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tas_tod_nof_bits_set;
    data_index = dnx_data_tsn_time_define_tas_tod_source;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tas_tod_source_set;

    

    
    
    submodule_index = dnx_data_tsn_submodule_tas;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_tas_define_max_time_intervals_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tas_max_time_intervals_entries_set;
    data_index = dnx_data_tsn_tas_define_nof_tas_ports;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tas_nof_tas_ports_set;
    data_index = dnx_data_tsn_tas_define_max_profiles_per_port;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tas_max_profiles_per_port_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

