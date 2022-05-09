
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
    SHR_IF_ERR_EXIT(dnx_data_tsn_tas_init(unit, &module_data->submodules[dnx_data_tsn_submodule_tas]));
    

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

