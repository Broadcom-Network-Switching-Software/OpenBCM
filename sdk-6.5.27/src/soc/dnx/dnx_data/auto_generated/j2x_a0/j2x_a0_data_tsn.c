
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
#include <soc/dnx/intr/auto_generated/j2x/j2x_intr.h>







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
j2x_a0_dnx_data_tsn_general_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_general;
    int feature_index = dnx_data_tsn_general_power_down;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_general_tsn_thread_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_general;
    int feature_index = dnx_data_tsn_general_tsn_thread;
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
j2x_a0_dnx_data_tsn_time_counters_atomic_read_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int feature_index = dnx_data_tsn_time_counters_atomic_read;
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
j2x_a0_dnx_data_tsn_time_tsn_counter_max_diff_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tsn_counter_max_diff;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_time_tas_tod_update_interrupt_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_time;
    int define_index = dnx_data_tsn_time_define_tas_tod_update_interrupt_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2X_INT_ECI_NANOSYNC_TAS_TOD_UPDATE;

    
    define->data = J2X_INT_ECI_NANOSYNC_TAS_TOD_UPDATE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_tsn_taf_max_time_intervals_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_taf;
    int define_index = dnx_data_tsn_taf_define_max_time_intervals_entries;
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
j2x_a0_dnx_data_tsn_taf_nof_gates_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_taf;
    int define_index = dnx_data_tsn_taf_define_nof_gates;
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
j2x_a0_dnx_data_tsn_taf_max_profiles_per_gate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_taf;
    int define_index = dnx_data_tsn_taf_define_max_profiles_per_gate;
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
j2x_a0_dnx_data_tsn_taf_gate_control_list_nof_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_taf;
    int define_index = dnx_data_tsn_taf_define_gate_control_list_nof_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16 * 1024;

    
    define->data = 16 * 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_taf_taf_admission_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_taf;
    int define_index = dnx_data_tsn_taf_define_taf_admission_profile;
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
j2x_a0_dnx_data_tsn_tas_max_time_intervals_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tas;
    int define_index = dnx_data_tsn_tas_define_max_time_intervals_entries;
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
j2x_a0_dnx_data_tsn_tas_nof_tas_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tas;
    int define_index = dnx_data_tsn_tas_define_nof_tas_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit) + 1;

    
    define->data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit) + 1;

    
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







static shr_error_e
j2x_a0_dnx_data_tsn_cqf_max_time_intervals_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_cqf;
    int define_index = dnx_data_tsn_cqf_define_max_time_intervals_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_cqf_nof_cqf_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_cqf;
    int define_index = dnx_data_tsn_cqf_define_nof_cqf_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit) + 1;

    
    define->data = dnx_data_egr_queuing.params.egr_if_nif_alloc_end_get(unit) - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit) + 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_cqf_time_margin_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_cqf;
    int define_index = dnx_data_tsn_cqf_define_time_margin;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50000000;

    
    define->data = 50000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_tsn_cqf_port_control_list_init_set(
    int unit)
{
    int idx_index;
    dnx_data_tsn_cqf_port_control_list_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_cqf;
    int table_index = dnx_data_tsn_cqf_table_port_control_list_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);
    table->info_get.key_size[0] = dnx_data_tsn.cqf.max_time_intervals_entries_get(unit);

    
    table->values[0].default_val = "0xFC";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_tsn_cqf_port_control_list_init_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_tsn_cqf_table_port_control_list_init");

    
    default_data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->gate_state = 0xFC;
    default_data->phase = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->gate_state = 0xFC;
        data->phase = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->gate_state = 0xFD;
        data->phase = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->gate_state = 0xFC;
        data->phase = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->gate_state = 0xFC;
        data->phase = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->gate_state = 0xFE;
        data->phase = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->gate_state = 0xFC;
        data->phase = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->gate_state = 0xFC;
        data->phase = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->gate_state = 0xFD;
        data->phase = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->gate_state = 0xFC;
        data->phase = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->gate_state = 0xFC;
        data->phase = 3;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->gate_state = 0xFE;
        data->phase = 3;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->gate_state = 0xFC;
        data->phase = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_tsn_tsn_thread_wakeup_interval_nsec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tsn_thread;
    int define_index = dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1100000000;

    
    define->data = 1100000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_tsn_thread_max_process_time_nsec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tsn_thread;
    int define_index = dnx_data_tsn_tsn_thread_define_max_process_time_nsec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 900000000;

    
    define->data = 900000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_tsn_tsn_thread_group_window_length_nsec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tsn;
    int submodule_index = dnx_data_tsn_submodule_tsn_thread;
    int define_index = dnx_data_tsn_tsn_thread_define_group_window_length_nsec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 900000000;

    
    define->data = 900000000;

    
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
    data_index = dnx_data_tsn_general_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_tsn_general_power_down_set;
    data_index = dnx_data_tsn_general_tsn_thread;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_tsn_general_tsn_thread_set;

    
    
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
    data_index = dnx_data_tsn_time_define_tsn_counter_max_diff;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tsn_counter_max_diff_set;
    data_index = dnx_data_tsn_time_define_tas_tod_update_interrupt_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_time_tas_tod_update_interrupt_id_set;

    
    data_index = dnx_data_tsn_time_counters_atomic_read;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_tsn_time_counters_atomic_read_set;

    
    
    submodule_index = dnx_data_tsn_submodule_taf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_taf_define_max_time_intervals_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_taf_max_time_intervals_entries_set;
    data_index = dnx_data_tsn_taf_define_nof_gates;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_taf_nof_gates_set;
    data_index = dnx_data_tsn_taf_define_max_profiles_per_gate;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_taf_max_profiles_per_gate_set;
    data_index = dnx_data_tsn_taf_define_gate_control_list_nof_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_taf_gate_control_list_nof_entries_set;
    data_index = dnx_data_tsn_taf_define_taf_admission_profile;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_taf_taf_admission_profile_set;

    

    
    
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

    

    
    
    submodule_index = dnx_data_tsn_submodule_cqf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_cqf_define_max_time_intervals_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_cqf_max_time_intervals_entries_set;
    data_index = dnx_data_tsn_cqf_define_nof_cqf_ports;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_cqf_nof_cqf_ports_set;
    data_index = dnx_data_tsn_cqf_define_time_margin;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_cqf_time_margin_set;

    

    
    data_index = dnx_data_tsn_cqf_table_port_control_list_init;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_tsn_cqf_port_control_list_init_set;
    
    submodule_index = dnx_data_tsn_submodule_tsn_thread;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tsn_thread_wakeup_interval_nsec_set;
    data_index = dnx_data_tsn_tsn_thread_define_max_process_time_nsec;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tsn_thread_max_process_time_nsec_set;
    data_index = dnx_data_tsn_tsn_thread_define_group_window_length_nsec;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_tsn_tsn_thread_group_window_length_nsec_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

