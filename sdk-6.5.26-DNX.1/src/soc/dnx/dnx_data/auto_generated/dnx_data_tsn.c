
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




extern shr_error_e j2x_a0_data_tsn_attach(
    int unit);




static shr_error_e
dnx_data_tsn_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn general features");

    submodule_data->features[dnx_data_tsn_general_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_tsn_general_is_supported].doc = "";
    submodule_data->features[dnx_data_tsn_general_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_tsn_general_tsn_thread].name = "tsn_thread";
    submodule_data->features[dnx_data_tsn_general_tsn_thread].doc = "";
    submodule_data->features[dnx_data_tsn_general_tsn_thread].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_tsn_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn general defines");

    
    submodule_data->nof_tables = _dnx_data_tsn_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_general_feature_get(
    int unit,
    dnx_data_tsn_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_general, feature);
}











static shr_error_e
dnx_data_tsn_preemption_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "preemption";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_preemption_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn preemption features");

    
    submodule_data->nof_defines = _dnx_data_tsn_preemption_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn preemption defines");

    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_time].name = "verify_max_time";
    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_time].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_time].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_attempts].name = "verify_max_attempts";
    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_attempts].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_verify_max_attempts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_tx].name = "non_final_frag_size_tx";
    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_tx].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_tx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_tx].name = "final_frag_size_tx";
    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_tx].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_tx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_rx].name = "non_final_frag_size_rx";
    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_rx].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_non_final_frag_size_rx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_rx].name = "final_frag_size_rx";
    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_rx].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_final_frag_size_rx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit].name = "frag_size_bytes_in_hw_unit";
    submodule_data->defines[dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit].doc = "";
    
    submodule_data->defines[dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_preemption_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn preemption tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_preemption_feature_get(
    int unit,
    dnx_data_tsn_preemption_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, feature);
}


uint32
dnx_data_tsn_preemption_verify_max_time_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_verify_max_time);
}

uint32
dnx_data_tsn_preemption_verify_max_attempts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_verify_max_attempts);
}

uint32
dnx_data_tsn_preemption_non_final_frag_size_tx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_non_final_frag_size_tx);
}

uint32
dnx_data_tsn_preemption_final_frag_size_tx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_final_frag_size_tx);
}

uint32
dnx_data_tsn_preemption_non_final_frag_size_rx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_non_final_frag_size_rx);
}

uint32
dnx_data_tsn_preemption_final_frag_size_rx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_final_frag_size_rx);
}

uint32
dnx_data_tsn_preemption_frag_size_bytes_in_hw_unit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_preemption, dnx_data_tsn_preemption_define_frag_size_bytes_in_hw_unit);
}










static shr_error_e
dnx_data_tsn_time_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "time";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_time_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn time features");

    
    submodule_data->nof_defines = _dnx_data_tsn_time_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn time defines");

    submodule_data->defines[dnx_data_tsn_time_define_tick_period_ns].name = "tick_period_ns";
    submodule_data->defines[dnx_data_tsn_time_define_tick_period_ns].doc = "";
    
    submodule_data->defines[dnx_data_tsn_time_define_tick_period_ns].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_time_define_tick_period_min_ns].name = "tick_period_min_ns";
    submodule_data->defines[dnx_data_tsn_time_define_tick_period_min_ns].doc = "";
    
    submodule_data->defines[dnx_data_tsn_time_define_tick_period_min_ns].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_time_define_tsn_counter_nof_bits].name = "tsn_counter_nof_bits";
    submodule_data->defines[dnx_data_tsn_time_define_tsn_counter_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_tsn_time_define_tsn_counter_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_nof_bits].name = "tas_tod_nof_bits";
    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_source].name = "tas_tod_source";
    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_source].doc = "";
    
    submodule_data->defines[dnx_data_tsn_time_define_tas_tod_source].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_time_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn time tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_time_feature_get(
    int unit,
    dnx_data_tsn_time_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, feature);
}


uint32
dnx_data_tsn_time_tick_period_ns_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, dnx_data_tsn_time_define_tick_period_ns);
}

uint32
dnx_data_tsn_time_tick_period_min_ns_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, dnx_data_tsn_time_define_tick_period_min_ns);
}

uint32
dnx_data_tsn_time_tsn_counter_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, dnx_data_tsn_time_define_tsn_counter_nof_bits);
}

uint32
dnx_data_tsn_time_tas_tod_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, dnx_data_tsn_time_define_tas_tod_nof_bits);
}

uint32
dnx_data_tsn_time_tas_tod_source_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_time, dnx_data_tsn_time_define_tas_tod_source);
}










static shr_error_e
dnx_data_tsn_taf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "taf";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_taf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn taf features");

    
    submodule_data->nof_defines = _dnx_data_tsn_taf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn taf defines");

    submodule_data->defines[dnx_data_tsn_taf_define_max_time_intervals_entries].name = "max_time_intervals_entries";
    submodule_data->defines[dnx_data_tsn_taf_define_max_time_intervals_entries].doc = "";
    
    submodule_data->defines[dnx_data_tsn_taf_define_max_time_intervals_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_taf_define_nof_gates].name = "nof_gates";
    submodule_data->defines[dnx_data_tsn_taf_define_nof_gates].doc = "";
    
    submodule_data->defines[dnx_data_tsn_taf_define_nof_gates].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_taf_define_max_profiles_per_gate].name = "max_profiles_per_gate";
    submodule_data->defines[dnx_data_tsn_taf_define_max_profiles_per_gate].doc = "";
    
    submodule_data->defines[dnx_data_tsn_taf_define_max_profiles_per_gate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_taf_define_gate_control_list_nof_entries].name = "gate_control_list_nof_entries";
    submodule_data->defines[dnx_data_tsn_taf_define_gate_control_list_nof_entries].doc = "";
    
    submodule_data->defines[dnx_data_tsn_taf_define_gate_control_list_nof_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_taf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn taf tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_taf_feature_get(
    int unit,
    dnx_data_tsn_taf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_taf, feature);
}


uint32
dnx_data_tsn_taf_max_time_intervals_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_taf, dnx_data_tsn_taf_define_max_time_intervals_entries);
}

uint32
dnx_data_tsn_taf_nof_gates_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_taf, dnx_data_tsn_taf_define_nof_gates);
}

uint32
dnx_data_tsn_taf_max_profiles_per_gate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_taf, dnx_data_tsn_taf_define_max_profiles_per_gate);
}

uint32
dnx_data_tsn_taf_gate_control_list_nof_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_taf, dnx_data_tsn_taf_define_gate_control_list_nof_entries);
}










static shr_error_e
dnx_data_tsn_tas_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tas";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_tas_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn tas features");

    
    submodule_data->nof_defines = _dnx_data_tsn_tas_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn tas defines");

    submodule_data->defines[dnx_data_tsn_tas_define_max_time_intervals_entries].name = "max_time_intervals_entries";
    submodule_data->defines[dnx_data_tsn_tas_define_max_time_intervals_entries].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tas_define_max_time_intervals_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_tas_define_nof_tas_ports].name = "nof_tas_ports";
    submodule_data->defines[dnx_data_tsn_tas_define_nof_tas_ports].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tas_define_nof_tas_ports].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_tas_define_max_profiles_per_port].name = "max_profiles_per_port";
    submodule_data->defines[dnx_data_tsn_tas_define_max_profiles_per_port].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tas_define_max_profiles_per_port].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_tas_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn tas tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_tas_feature_get(
    int unit,
    dnx_data_tsn_tas_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tas, feature);
}


uint32
dnx_data_tsn_tas_max_time_intervals_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tas, dnx_data_tsn_tas_define_max_time_intervals_entries);
}

uint32
dnx_data_tsn_tas_nof_tas_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tas, dnx_data_tsn_tas_define_nof_tas_ports);
}

uint32
dnx_data_tsn_tas_max_profiles_per_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tas, dnx_data_tsn_tas_define_max_profiles_per_port);
}










static shr_error_e
dnx_data_tsn_cqf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cqf";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_cqf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn cqf features");

    
    submodule_data->nof_defines = _dnx_data_tsn_cqf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn cqf defines");

    submodule_data->defines[dnx_data_tsn_cqf_define_max_time_intervals_entries].name = "max_time_intervals_entries";
    submodule_data->defines[dnx_data_tsn_cqf_define_max_time_intervals_entries].doc = "";
    
    submodule_data->defines[dnx_data_tsn_cqf_define_max_time_intervals_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_cqf_define_nof_cqf_ports].name = "nof_cqf_ports";
    submodule_data->defines[dnx_data_tsn_cqf_define_nof_cqf_ports].doc = "";
    
    submodule_data->defines[dnx_data_tsn_cqf_define_nof_cqf_ports].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_cqf_define_time_margin].name = "time_margin";
    submodule_data->defines[dnx_data_tsn_cqf_define_time_margin].doc = "";
    
    submodule_data->defines[dnx_data_tsn_cqf_define_time_margin].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_cqf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn cqf tables");

    
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].name = "port_control_list_init";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].doc = "";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].size_of_values = sizeof(dnx_data_tsn_cqf_port_control_list_init_t);
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].entry_get = dnx_data_tsn_cqf_port_control_list_init_entry_str_get;

    
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].nof_keys = 1;
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].keys[0].name = "idx";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].nof_values, "_dnx_data_tsn_cqf_table_port_control_list_init table values");
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[0].name = "gate_state";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[0].type = "int";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[0].doc = "";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_tsn_cqf_port_control_list_init_t, gate_state);
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[1].name = "phase";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[1].type = "int";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[1].doc = "";
    submodule_data->tables[dnx_data_tsn_cqf_table_port_control_list_init].values[1].offset = UTILEX_OFFSETOF(dnx_data_tsn_cqf_port_control_list_init_t, phase);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_cqf_feature_get(
    int unit,
    dnx_data_tsn_cqf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, feature);
}


uint32
dnx_data_tsn_cqf_max_time_intervals_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_define_max_time_intervals_entries);
}

uint32
dnx_data_tsn_cqf_nof_cqf_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_define_nof_cqf_ports);
}

uint32
dnx_data_tsn_cqf_time_margin_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_define_time_margin);
}



const dnx_data_tsn_cqf_port_control_list_init_t *
dnx_data_tsn_cqf_port_control_list_init_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_table_port_control_list_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_tsn_cqf_port_control_list_init_t *) data;

}


shr_error_e
dnx_data_tsn_cqf_port_control_list_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_tsn_cqf_port_control_list_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_table_port_control_list_init);
    data = (const dnx_data_tsn_cqf_port_control_list_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->gate_state);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->phase);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_tsn_cqf_port_control_list_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_cqf, dnx_data_tsn_cqf_table_port_control_list_init);

}






static shr_error_e
dnx_data_tsn_tsn_thread_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tsn_thread";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_tsn_tsn_thread_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tsn tsn_thread features");

    
    submodule_data->nof_defines = _dnx_data_tsn_tsn_thread_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tsn tsn_thread defines");

    submodule_data->defines[dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec].name = "wakeup_interval_nsec";
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_tsn_thread_define_max_process_time_nsec].name = "max_process_time_nsec";
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_max_process_time_nsec].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_max_process_time_nsec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tsn_tsn_thread_define_group_window_length_nsec].name = "group_window_length_nsec";
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_group_window_length_nsec].doc = "";
    
    submodule_data->defines[dnx_data_tsn_tsn_thread_define_group_window_length_nsec].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_tsn_tsn_thread_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tsn tsn_thread tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tsn_tsn_thread_feature_get(
    int unit,
    dnx_data_tsn_tsn_thread_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tsn_thread, feature);
}


uint32
dnx_data_tsn_tsn_thread_wakeup_interval_nsec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tsn_thread, dnx_data_tsn_tsn_thread_define_wakeup_interval_nsec);
}

uint32
dnx_data_tsn_tsn_thread_max_process_time_nsec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tsn_thread, dnx_data_tsn_tsn_thread_define_max_process_time_nsec);
}

uint32
dnx_data_tsn_tsn_thread_group_window_length_nsec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tsn, dnx_data_tsn_submodule_tsn_thread, dnx_data_tsn_tsn_thread_define_group_window_length_nsec);
}







shr_error_e
dnx_data_tsn_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "tsn";
    module_data->nof_submodules = _dnx_data_tsn_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data tsn submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_tsn_general_init(unit, &module_data->submodules[dnx_data_tsn_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_preemption_init(unit, &module_data->submodules[dnx_data_tsn_submodule_preemption]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_time_init(unit, &module_data->submodules[dnx_data_tsn_submodule_time]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_taf_init(unit, &module_data->submodules[dnx_data_tsn_submodule_taf]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_tas_init(unit, &module_data->submodules[dnx_data_tsn_submodule_tas]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_cqf_init(unit, &module_data->submodules[dnx_data_tsn_submodule_cqf]));
    SHR_IF_ERR_EXIT(dnx_data_tsn_tsn_thread_init(unit, &module_data->submodules[dnx_data_tsn_submodule_tsn_thread]));
    

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(j2x_a0_data_tsn_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

