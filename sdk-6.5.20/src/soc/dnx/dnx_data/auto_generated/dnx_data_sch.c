

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sch.h>



extern shr_error_e jr2_a0_data_sch_attach(
    int unit);
extern shr_error_e j2c_a0_data_sch_attach(
    int unit);
extern shr_error_e q2a_a0_data_sch_attach(
    int unit);
extern shr_error_e j2p_a0_data_sch_attach(
    int unit);



static shr_error_e
dnx_data_sch_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "e2e scheduler general information";
    
    submodule_data->nof_features = _dnx_data_sch_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch general features");

    submodule_data->features[dnx_data_sch_general_lag_scheduler_supported].name = "lag_scheduler_supported";
    submodule_data->features[dnx_data_sch_general_lag_scheduler_supported].doc = "Is LAG scheduler supported";
    submodule_data->features[dnx_data_sch_general_lag_scheduler_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sch_general_fsm_modes_config_supported].name = "fsm_modes_config_supported";
    submodule_data->features[dnx_data_sch_general_fsm_modes_config_supported].doc = "Fabric Status Message modes configuration supported";
    submodule_data->features[dnx_data_sch_general_fsm_modes_config_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sch_general_calrx_rxi_enable_supported].name = "calrx_rxi_enable_supported";
    submodule_data->features[dnx_data_sch_general_calrx_rxi_enable_supported].doc = "Calrx and rxi blocks enable/disable is supported";
    submodule_data->features[dnx_data_sch_general_calrx_rxi_enable_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sch_general_port_priority_propagation_supported].name = "port_priority_propagation_supported";
    submodule_data->features[dnx_data_sch_general_port_priority_propagation_supported].doc = "Is Port priority propagation supported";
    submodule_data->features[dnx_data_sch_general_port_priority_propagation_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sch_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch general defines");

    submodule_data->defines[dnx_data_sch_general_define_nof_slow_profiles].name = "nof_slow_profiles";
    submodule_data->defines[dnx_data_sch_general_define_nof_slow_profiles].doc = "Number of slow profiles";
    
    submodule_data->defines[dnx_data_sch_general_define_nof_slow_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_general_define_nof_slow_levels].name = "nof_slow_levels";
    submodule_data->defines[dnx_data_sch_general_define_nof_slow_levels].doc = "Number of slow levels";
    
    submodule_data->defines[dnx_data_sch_general_define_nof_slow_levels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_general_define_cycles_per_credit_token].name = "cycles_per_credit_token";
    submodule_data->defines[dnx_data_sch_general_define_cycles_per_credit_token].doc = "Number of of cycles between credit tokens of the same flow";
    
    submodule_data->defines[dnx_data_sch_general_define_cycles_per_credit_token].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_general_define_nof_fmq_class].name = "nof_fmq_class";
    submodule_data->defines[dnx_data_sch_general_define_nof_fmq_class].doc = "Number of FMQ class";
    
    submodule_data->defines[dnx_data_sch_general_define_nof_fmq_class].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_general_define_port_priority_propagation_enable].name = "port_priority_propagation_enable";
    submodule_data->defines[dnx_data_sch_general_define_port_priority_propagation_enable].doc = "Enable ports to be created with port priority propagation";
    
    submodule_data->defines[dnx_data_sch_general_define_port_priority_propagation_enable].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch general tables");

    
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].name = "low_rate_factor_to_dbal_enum";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].doc = "mapping low rate factor to dbal enum";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].size_of_values = sizeof(dnx_data_sch_general_low_rate_factor_to_dbal_enum_t);
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].entry_get = dnx_data_sch_general_low_rate_factor_to_dbal_enum_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].keys[0].name = "low_rate_factor";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].keys[0].doc = "low rate factor";

    
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].nof_values, "_dnx_data_sch_general_table_low_rate_factor_to_dbal_enum table values");
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[0].name = "dbal_enum";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[0].type = "dbal_enum_value_field_low_rate_factor_e";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[0].doc = "low rate factor dbal enum";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_general_low_rate_factor_to_dbal_enum_t, dbal_enum);
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[1].name = "valid";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[1].type = "int";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[1].doc = "is factor valid";
    submodule_data->tables[dnx_data_sch_general_table_low_rate_factor_to_dbal_enum].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_general_low_rate_factor_to_dbal_enum_t, valid);

    
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].name = "dbal_enum_to_low_rate_factor";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].doc = "mapping dbal enum to low rate factor";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].size_of_values = sizeof(dnx_data_sch_general_dbal_enum_to_low_rate_factor_t);
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].entry_get = dnx_data_sch_general_dbal_enum_to_low_rate_factor_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].keys[0].name = "dbal_enum";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].keys[0].doc = "low rate factor dbal enum";

    
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].nof_values, "_dnx_data_sch_general_table_dbal_enum_to_low_rate_factor table values");
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].values[0].name = "low_rate_factor";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].values[0].type = "int";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].values[0].doc = "low rate factor";
    submodule_data->tables[dnx_data_sch_general_table_dbal_enum_to_low_rate_factor].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_general_dbal_enum_to_low_rate_factor_t, low_rate_factor);

    
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].name = "slow_rate_max_bucket";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].doc = "Max token bucket values per slow rate";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].size_of_values = sizeof(dnx_data_sch_general_slow_rate_max_bucket_t);
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].entry_get = dnx_data_sch_general_slow_rate_max_bucket_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].keys[0].name = "idx";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].keys[0].doc = "table index";

    
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].nof_values, "_dnx_data_sch_general_table_slow_rate_max_bucket table values");
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[0].name = "rate";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[0].type = "int";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[0].doc = "Slow rate [Mbps]";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_general_slow_rate_max_bucket_t, rate);
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[1].name = "max_bucket";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[1].doc = "Max token bucket";
    submodule_data->tables[dnx_data_sch_general_table_slow_rate_max_bucket].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_general_slow_rate_max_bucket_t, max_bucket);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_general_feature_get(
    int unit,
    dnx_data_sch_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, feature);
}


uint32
dnx_data_sch_general_nof_slow_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_define_nof_slow_profiles);
}

uint32
dnx_data_sch_general_nof_slow_levels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_define_nof_slow_levels);
}

uint32
dnx_data_sch_general_cycles_per_credit_token_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_define_cycles_per_credit_token);
}

uint32
dnx_data_sch_general_nof_fmq_class_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_define_nof_fmq_class);
}

uint32
dnx_data_sch_general_port_priority_propagation_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_define_port_priority_propagation_enable);
}



const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *
dnx_data_sch_general_low_rate_factor_to_dbal_enum_get(
    int unit,
    int low_rate_factor)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_low_rate_factor_to_dbal_enum);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, low_rate_factor, 0);
    return (const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) data;

}

const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *
dnx_data_sch_general_dbal_enum_to_low_rate_factor_get(
    int unit,
    int dbal_enum)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_dbal_enum_to_low_rate_factor);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dbal_enum, 0);
    return (const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) data;

}

const dnx_data_sch_general_slow_rate_max_bucket_t *
dnx_data_sch_general_slow_rate_max_bucket_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_slow_rate_max_bucket);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_sch_general_slow_rate_max_bucket_t *) data;

}


shr_error_e
dnx_data_sch_general_low_rate_factor_to_dbal_enum_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_low_rate_factor_to_dbal_enum);
    data = (const dnx_data_sch_general_low_rate_factor_to_dbal_enum_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_enum);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_general_dbal_enum_to_low_rate_factor_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_dbal_enum_to_low_rate_factor);
    data = (const dnx_data_sch_general_dbal_enum_to_low_rate_factor_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->low_rate_factor);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_general_slow_rate_max_bucket_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_general_slow_rate_max_bucket_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_slow_rate_max_bucket);
    data = (const dnx_data_sch_general_slow_rate_max_bucket_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rate);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_bucket);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_sch_general_low_rate_factor_to_dbal_enum_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_low_rate_factor_to_dbal_enum);

}

const dnxc_data_table_info_t *
dnx_data_sch_general_dbal_enum_to_low_rate_factor_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_dbal_enum_to_low_rate_factor);

}

const dnxc_data_table_info_t *
dnx_data_sch_general_slow_rate_max_bucket_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_general, dnx_data_sch_general_table_slow_rate_max_bucket);

}






static shr_error_e
dnx_data_sch_ps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ps";
    submodule_data->doc = "e2e port scheduler (PS) information";
    
    submodule_data->nof_features = _dnx_data_sch_ps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch ps features");

    
    submodule_data->nof_defines = _dnx_data_sch_ps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch ps defines");

    submodule_data->defines[dnx_data_sch_ps_define_min_priority_for_tcg].name = "min_priority_for_tcg";
    submodule_data->defines[dnx_data_sch_ps_define_min_priority_for_tcg].doc = "Minimal port priority supporting tcg";
    
    submodule_data->defines[dnx_data_sch_ps_define_min_priority_for_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_ps_define_max_nof_tcg].name = "max_nof_tcg";
    submodule_data->defines[dnx_data_sch_ps_define_max_nof_tcg].doc = "Maximal number of TCGs in a port scheduler";
    
    submodule_data->defines[dnx_data_sch_ps_define_max_nof_tcg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_ps_define_nof_hrs_in_ps].name = "nof_hrs_in_ps";
    submodule_data->defines[dnx_data_sch_ps_define_nof_hrs_in_ps].doc = "Number of HRs in port scheduler";
    
    submodule_data->defines[dnx_data_sch_ps_define_nof_hrs_in_ps].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_ps_define_max_tcg_weight].name = "max_tcg_weight";
    submodule_data->defines[dnx_data_sch_ps_define_max_tcg_weight].doc = "Maximal TCG EIR weight";
    
    submodule_data->defines[dnx_data_sch_ps_define_max_tcg_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_ps_define_tc_tcg_calendar_access_period].name = "tc_tcg_calendar_access_period";
    submodule_data->defines[dnx_data_sch_ps_define_tc_tcg_calendar_access_period].doc = "Access period of TC/TCG calendars";
    
    submodule_data->defines[dnx_data_sch_ps_define_tc_tcg_calendar_access_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_ps_define_max_port_rate_mbps].name = "max_port_rate_mbps";
    submodule_data->defines[dnx_data_sch_ps_define_max_port_rate_mbps].doc = "maximal rate an port can drive (in MBits/sec)";
    
    submodule_data->defines[dnx_data_sch_ps_define_max_port_rate_mbps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_ps_define_max_burst].name = "max_burst";
    submodule_data->defines[dnx_data_sch_ps_define_max_burst].doc = "Maximal burst of TC/TCG port shapers";
    
    submodule_data->defines[dnx_data_sch_ps_define_max_burst].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_ps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch ps tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_ps_feature_get(
    int unit,
    dnx_data_sch_ps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, feature);
}


uint32
dnx_data_sch_ps_min_priority_for_tcg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_min_priority_for_tcg);
}

uint32
dnx_data_sch_ps_max_nof_tcg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_max_nof_tcg);
}

uint32
dnx_data_sch_ps_nof_hrs_in_ps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_nof_hrs_in_ps);
}

uint32
dnx_data_sch_ps_max_tcg_weight_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_max_tcg_weight);
}

uint32
dnx_data_sch_ps_tc_tcg_calendar_access_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_tc_tcg_calendar_access_period);
}

uint32
dnx_data_sch_ps_max_port_rate_mbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_max_port_rate_mbps);
}

uint32
dnx_data_sch_ps_max_burst_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_ps, dnx_data_sch_ps_define_max_burst);
}










static shr_error_e
dnx_data_sch_flow_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flow";
    submodule_data->doc = "e2e scheduler information";
    
    submodule_data->nof_features = _dnx_data_sch_flow_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch flow features");

    submodule_data->features[dnx_data_sch_flow_cl_fq_cl_fq_quartet].name = "cl_fq_cl_fq_quartet";
    submodule_data->features[dnx_data_sch_flow_cl_fq_cl_fq_quartet].doc = "Is Cl-FQ-Cl-FQ quartet supported";
    submodule_data->features[dnx_data_sch_flow_cl_fq_cl_fq_quartet].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sch_flow_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch flow defines");

    submodule_data->defines[dnx_data_sch_flow_define_nof_hr].name = "nof_hr";
    submodule_data->defines[dnx_data_sch_flow_define_nof_hr].doc = "Number of HR elements per core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_hr].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_cl].name = "nof_cl";
    submodule_data->defines[dnx_data_sch_flow_define_nof_cl].doc = "Number of CL elements per core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_cl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_se].name = "nof_se";
    submodule_data->defines[dnx_data_sch_flow_define_nof_se].doc = "Number of scheduling elements per core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_se].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_flows].name = "nof_flows";
    submodule_data->defines[dnx_data_sch_flow_define_nof_flows].doc = "Number of flows per core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_flows].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_region_size].name = "region_size";
    submodule_data->defines[dnx_data_sch_flow_define_region_size].doc = "Number of flows in a region";
    
    submodule_data->defines[dnx_data_sch_flow_define_region_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_regions].name = "nof_regions";
    submodule_data->defines[dnx_data_sch_flow_define_nof_regions].doc = "Number of regions in a core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_regions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_hr_regions].name = "nof_hr_regions";
    submodule_data->defines[dnx_data_sch_flow_define_nof_hr_regions].doc = "Number of hr regions in a core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_hr_regions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_nof_connectors_only_regions].name = "nof_connectors_only_regions";
    submodule_data->defines[dnx_data_sch_flow_define_nof_connectors_only_regions].doc = "Number of connectors-only regions per core";
    
    submodule_data->defines[dnx_data_sch_flow_define_nof_connectors_only_regions].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_flow_in_region_mask].name = "flow_in_region_mask";
    submodule_data->defines[dnx_data_sch_flow_define_flow_in_region_mask].doc = "mask for flows in region";
    
    submodule_data->defines[dnx_data_sch_flow_define_flow_in_region_mask].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_min_connector_bundle_size].name = "min_connector_bundle_size";
    submodule_data->defines[dnx_data_sch_flow_define_min_connector_bundle_size].doc = "minimal number of connectors in bundle";
    
    submodule_data->defines[dnx_data_sch_flow_define_min_connector_bundle_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_conn_idx_interdig_flow].name = "conn_idx_interdig_flow";
    submodule_data->defines[dnx_data_sch_flow_define_conn_idx_interdig_flow].doc = "first connector index in an interdigitated flow";
    
    submodule_data->defines[dnx_data_sch_flow_define_conn_idx_interdig_flow].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_shaper_flipflops_nof].name = "shaper_flipflops_nof";
    submodule_data->defines[dnx_data_sch_flow_define_shaper_flipflops_nof].doc = "number of Flip-Flops in flow shaper implementation in HW.                     if exp greater than (shaper_flipflops_nof - nof_manthissa_bits)) then mantissa[exp-(shaper_flipflops_nof-nof_mantissa_bits)-1: 0] == 0";
    
    submodule_data->defines[dnx_data_sch_flow_define_shaper_flipflops_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_flow_define_erp_hr].name = "erp_hr";
    submodule_data->defines[dnx_data_sch_flow_define_erp_hr].doc = "HR id of first ERP HR";
    
    submodule_data->defines[dnx_data_sch_flow_define_erp_hr].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_reserved_hr].name = "reserved_hr";
    submodule_data->defines[dnx_data_sch_flow_define_reserved_hr].doc = "reserved HR allocated for returned credits in LAG scheduling";
    
    submodule_data->defines[dnx_data_sch_flow_define_reserved_hr].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_max_se].name = "max_se";
    submodule_data->defines[dnx_data_sch_flow_define_max_se].doc = "Maximal Scheduling element id";
    
    submodule_data->defines[dnx_data_sch_flow_define_max_se].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_max_flow].name = "max_flow";
    submodule_data->defines[dnx_data_sch_flow_define_max_flow].doc = "Maximal flow id";
    
    submodule_data->defines[dnx_data_sch_flow_define_max_flow].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_first_hr_region].name = "first_hr_region";
    submodule_data->defines[dnx_data_sch_flow_define_first_hr_region].doc = "First HR region";
    
    submodule_data->defines[dnx_data_sch_flow_define_first_hr_region].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_first_se_flow_id].name = "first_se_flow_id";
    submodule_data->defines[dnx_data_sch_flow_define_first_se_flow_id].doc = "Flow ID of first scheduling element";
    
    submodule_data->defines[dnx_data_sch_flow_define_first_se_flow_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_min].name = "hr_se_id_min";
    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_min].doc = "min hr se id";
    
    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_min].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_max].name = "hr_se_id_max";
    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_max].doc = "max hr se id";
    
    submodule_data->defines[dnx_data_sch_flow_define_hr_se_id_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_default_credit_source_se_id].name = "default_credit_source_se_id";
    submodule_data->defines[dnx_data_sch_flow_define_default_credit_source_se_id].doc = "SE ID of default credit source. used to mark unattached flow";
    
    submodule_data->defines[dnx_data_sch_flow_define_default_credit_source_se_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation].name = "runtime_performance_optimize_enable_sched_allocation";
    submodule_data->defines[dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation].doc = "enable flow properties access optimization";
    
    submodule_data->defines[dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_flow_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch flow tables");

    
    submodule_data->tables[dnx_data_sch_flow_table_region_type].name = "region_type";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].doc = "flow region type";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_flow_table_region_type].size_of_values = sizeof(dnx_data_sch_flow_region_type_t);
    submodule_data->tables[dnx_data_sch_flow_table_region_type].entry_get = dnx_data_sch_flow_region_type_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_flow_table_region_type].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_flow_table_region_type].keys[0].name = "core";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].keys[0].doc = "see table doc";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].keys[1].name = "region";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].keys[1].doc = "see table doc";

    
    submodule_data->tables[dnx_data_sch_flow_table_region_type].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_flow_table_region_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_flow_table_region_type].nof_values, "_dnx_data_sch_flow_table_region_type table values");
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[0].name = "type";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[0].doc = "see table doc";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_flow_region_type_t, type);
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[1].name = "propagation_direction";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[1].doc = "see table doc";
    submodule_data->tables[dnx_data_sch_flow_table_region_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_flow_region_type_t, propagation_direction);

    
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].name = "nof_remote_cores";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].doc = "flow region type";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].size_of_values = sizeof(dnx_data_sch_flow_nof_remote_cores_t);
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].entry_get = dnx_data_sch_flow_nof_remote_cores_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].keys[0].name = "core";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].keys[0].doc = "see table doc";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].keys[1].name = "region";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].keys[1].doc = "see table doc";

    
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].nof_values, "_dnx_data_sch_flow_table_nof_remote_cores table values");
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].values[0].name = "val";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].values[0].doc = "see table doc";
    submodule_data->tables[dnx_data_sch_flow_table_nof_remote_cores].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_flow_nof_remote_cores_t, val);

    
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].name = "cl_num_to_quartet_offset";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].doc = "mapping CL number to quartet index";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].size_of_values = sizeof(dnx_data_sch_flow_cl_num_to_quartet_offset_t);
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].entry_get = dnx_data_sch_flow_cl_num_to_quartet_offset_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].keys[0].name = "cl_num";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].keys[0].doc = "number of CL in the quartet";

    
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].nof_values, "_dnx_data_sch_flow_table_cl_num_to_quartet_offset table values");
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].values[0].name = "quartet_offset";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].values[0].type = "int";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].values[0].doc = "quartet offset";
    submodule_data->tables[dnx_data_sch_flow_table_cl_num_to_quartet_offset].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_flow_cl_num_to_quartet_offset_t, quartet_offset);

    
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].name = "quartet_offset_to_cl_num";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].doc = "mapping quartet index to CL number";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].size_of_values = sizeof(dnx_data_sch_flow_quartet_offset_to_cl_num_t);
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].entry_get = dnx_data_sch_flow_quartet_offset_to_cl_num_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].keys[0].name = "quartet_offset";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].keys[0].doc = "quartet offset";

    
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].nof_values, "_dnx_data_sch_flow_table_quartet_offset_to_cl_num table values");
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].values[0].name = "cl_num";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].values[0].type = "int";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].values[0].doc = "number of CL in the quartet";
    submodule_data->tables[dnx_data_sch_flow_table_quartet_offset_to_cl_num].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_flow_quartet_offset_to_cl_num_t, cl_num);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_flow_feature_get(
    int unit,
    dnx_data_sch_flow_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, feature);
}


uint32
dnx_data_sch_flow_nof_hr_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_hr);
}

uint32
dnx_data_sch_flow_nof_cl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_cl);
}

uint32
dnx_data_sch_flow_nof_se_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_se);
}

uint32
dnx_data_sch_flow_nof_flows_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_flows);
}

uint32
dnx_data_sch_flow_region_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_region_size);
}

uint32
dnx_data_sch_flow_nof_regions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_regions);
}

uint32
dnx_data_sch_flow_nof_hr_regions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_hr_regions);
}

uint32
dnx_data_sch_flow_nof_connectors_only_regions_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_nof_connectors_only_regions);
}

uint32
dnx_data_sch_flow_flow_in_region_mask_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_flow_in_region_mask);
}

uint32
dnx_data_sch_flow_min_connector_bundle_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_min_connector_bundle_size);
}

uint32
dnx_data_sch_flow_conn_idx_interdig_flow_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_conn_idx_interdig_flow);
}

uint32
dnx_data_sch_flow_shaper_flipflops_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_shaper_flipflops_nof);
}

uint32
dnx_data_sch_flow_erp_hr_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_erp_hr);
}

uint32
dnx_data_sch_flow_reserved_hr_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_reserved_hr);
}

uint32
dnx_data_sch_flow_max_se_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_max_se);
}

uint32
dnx_data_sch_flow_max_flow_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_max_flow);
}

uint32
dnx_data_sch_flow_first_hr_region_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_first_hr_region);
}

uint32
dnx_data_sch_flow_first_se_flow_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_first_se_flow_id);
}

uint32
dnx_data_sch_flow_hr_se_id_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_hr_se_id_min);
}

uint32
dnx_data_sch_flow_hr_se_id_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_hr_se_id_max);
}

uint32
dnx_data_sch_flow_default_credit_source_se_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_default_credit_source_se_id);
}

uint32
dnx_data_sch_flow_runtime_performance_optimize_enable_sched_allocation_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_define_runtime_performance_optimize_enable_sched_allocation);
}



const dnx_data_sch_flow_region_type_t *
dnx_data_sch_flow_region_type_get(
    int unit,
    int core,
    int region)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_region_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core, region);
    return (const dnx_data_sch_flow_region_type_t *) data;

}

const dnx_data_sch_flow_nof_remote_cores_t *
dnx_data_sch_flow_nof_remote_cores_get(
    int unit,
    int core,
    int region)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_nof_remote_cores);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core, region);
    return (const dnx_data_sch_flow_nof_remote_cores_t *) data;

}

const dnx_data_sch_flow_cl_num_to_quartet_offset_t *
dnx_data_sch_flow_cl_num_to_quartet_offset_get(
    int unit,
    int cl_num)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_cl_num_to_quartet_offset);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, cl_num, 0);
    return (const dnx_data_sch_flow_cl_num_to_quartet_offset_t *) data;

}

const dnx_data_sch_flow_quartet_offset_to_cl_num_t *
dnx_data_sch_flow_quartet_offset_to_cl_num_get(
    int unit,
    int quartet_offset)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_quartet_offset_to_cl_num);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, quartet_offset, 0);
    return (const dnx_data_sch_flow_quartet_offset_to_cl_num_t *) data;

}


shr_error_e
dnx_data_sch_flow_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_flow_region_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_region_type);
    data = (const dnx_data_sch_flow_region_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->propagation_direction);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_flow_nof_remote_cores_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_flow_nof_remote_cores_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_nof_remote_cores);
    data = (const dnx_data_sch_flow_nof_remote_cores_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_flow_cl_num_to_quartet_offset_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_flow_cl_num_to_quartet_offset_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_cl_num_to_quartet_offset);
    data = (const dnx_data_sch_flow_cl_num_to_quartet_offset_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->quartet_offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_flow_quartet_offset_to_cl_num_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_flow_quartet_offset_to_cl_num_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_quartet_offset_to_cl_num);
    data = (const dnx_data_sch_flow_quartet_offset_to_cl_num_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cl_num);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_sch_flow_region_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_region_type);

}

const dnxc_data_table_info_t *
dnx_data_sch_flow_nof_remote_cores_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_nof_remote_cores);

}

const dnxc_data_table_info_t *
dnx_data_sch_flow_cl_num_to_quartet_offset_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_cl_num_to_quartet_offset);

}

const dnxc_data_table_info_t *
dnx_data_sch_flow_quartet_offset_to_cl_num_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_flow, dnx_data_sch_flow_table_quartet_offset_to_cl_num);

}






static shr_error_e
dnx_data_sch_se_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "se";
    submodule_data->doc = "scheduler elements information";
    
    submodule_data->nof_features = _dnx_data_sch_se_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch se features");

    
    submodule_data->nof_defines = _dnx_data_sch_se_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch se defines");

    submodule_data->defines[dnx_data_sch_se_define_default_cos].name = "default_cos";
    submodule_data->defines[dnx_data_sch_se_define_default_cos].doc = "default parameter of credit source COS. Used for FQ";
    
    submodule_data->defines[dnx_data_sch_se_define_default_cos].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_se_define_nof_color_group].name = "nof_color_group";
    submodule_data->defines[dnx_data_sch_se_define_nof_color_group].doc = "number of colors (groups)";
    
    submodule_data->defines[dnx_data_sch_se_define_nof_color_group].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_se_define_max_hr_weight].name = "max_hr_weight";
    submodule_data->defines[dnx_data_sch_se_define_max_hr_weight].doc = "maximal weight in HR WFQ";
    
    submodule_data->defines[dnx_data_sch_se_define_max_hr_weight].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_se_define_cl_class_profile_nof].name = "cl_class_profile_nof";
    submodule_data->defines[dnx_data_sch_se_define_cl_class_profile_nof].doc = "number of profiles for CL class";
    
    submodule_data->defines[dnx_data_sch_se_define_cl_class_profile_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_sch_se_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch se tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_se_feature_get(
    int unit,
    dnx_data_sch_se_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_se, feature);
}


uint32
dnx_data_sch_se_default_cos_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_se, dnx_data_sch_se_define_default_cos);
}

uint32
dnx_data_sch_se_nof_color_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_se, dnx_data_sch_se_define_nof_color_group);
}

uint32
dnx_data_sch_se_max_hr_weight_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_se, dnx_data_sch_se_define_max_hr_weight);
}

uint32
dnx_data_sch_se_cl_class_profile_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_se, dnx_data_sch_se_define_cl_class_profile_nof);
}










static shr_error_e
dnx_data_sch_interface_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "interface";
    submodule_data->doc = "e2e interface information";
    
    submodule_data->nof_features = _dnx_data_sch_interface_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch interface features");

    submodule_data->features[dnx_data_sch_interface_non_channelized_calendar].name = "non_channelized_calendar";
    submodule_data->features[dnx_data_sch_interface_non_channelized_calendar].doc = "Is not channelized calender supported";
    submodule_data->features[dnx_data_sch_interface_non_channelized_calendar].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sch_interface_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch interface defines");

    submodule_data->defines[dnx_data_sch_interface_define_nof_big_calendars].name = "nof_big_calendars";
    submodule_data->defines[dnx_data_sch_interface_define_nof_big_calendars].doc = "Number of big calendars per core";
    
    submodule_data->defines[dnx_data_sch_interface_define_nof_big_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_nof_channelized_calendars].name = "nof_channelized_calendars";
    submodule_data->defines[dnx_data_sch_interface_define_nof_channelized_calendars].doc = "Number of channelized calendars per core (both big and regular)";
    
    submodule_data->defines[dnx_data_sch_interface_define_nof_channelized_calendars].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_big_calendar_size].name = "big_calendar_size";
    submodule_data->defines[dnx_data_sch_interface_define_big_calendar_size].doc = "Size of big calendar";
    
    submodule_data->defines[dnx_data_sch_interface_define_big_calendar_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_nof_sch_interfaces].name = "nof_sch_interfaces";
    submodule_data->defines[dnx_data_sch_interface_define_nof_sch_interfaces].doc = "Number of sch interfaces per core";
    
    submodule_data->defines[dnx_data_sch_interface_define_nof_sch_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_cal_speed_resolution].name = "cal_speed_resolution";
    submodule_data->defines[dnx_data_sch_interface_define_cal_speed_resolution].doc = "Resolution of interface calendar processing speed";
    
    submodule_data->defines[dnx_data_sch_interface_define_cal_speed_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_reserved].name = "reserved";
    submodule_data->defines[dnx_data_sch_interface_define_reserved].doc = "Reserved SCH interface to disable interface level FC in case of LAG scheduling";
    
    submodule_data->defines[dnx_data_sch_interface_define_reserved].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_interface_define_max_if_rate_mbps].name = "max_if_rate_mbps";
    submodule_data->defines[dnx_data_sch_interface_define_max_if_rate_mbps].doc = "maximal rate an interface can drive (in MBits/sec)";
    
    submodule_data->defines[dnx_data_sch_interface_define_max_if_rate_mbps].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_interface_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch interface tables");

    
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].name = "shaper_resolution";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].doc = "shaper resolution for channelized/non channelized interface";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].size_of_values = sizeof(dnx_data_sch_interface_shaper_resolution_t);
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].entry_get = dnx_data_sch_interface_shaper_resolution_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].keys[0].name = "channelized";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].keys[0].doc = "is interface channelized";

    
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].nof_values, "_dnx_data_sch_interface_table_shaper_resolution table values");
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].values[0].name = "resolution";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].values[0].doc = "Resolution of interface shaper";
    submodule_data->tables[dnx_data_sch_interface_table_shaper_resolution].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_interface_shaper_resolution_t, resolution);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_interface_feature_get(
    int unit,
    dnx_data_sch_interface_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, feature);
}


uint32
dnx_data_sch_interface_nof_big_calendars_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_nof_big_calendars);
}

uint32
dnx_data_sch_interface_nof_channelized_calendars_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_nof_channelized_calendars);
}

uint32
dnx_data_sch_interface_big_calendar_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_big_calendar_size);
}

uint32
dnx_data_sch_interface_nof_sch_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_nof_sch_interfaces);
}

uint32
dnx_data_sch_interface_cal_speed_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_cal_speed_resolution);
}

uint32
dnx_data_sch_interface_reserved_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_reserved);
}

uint32
dnx_data_sch_interface_max_if_rate_mbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_define_max_if_rate_mbps);
}



const dnx_data_sch_interface_shaper_resolution_t *
dnx_data_sch_interface_shaper_resolution_get(
    int unit,
    int channelized)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_table_shaper_resolution);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, channelized, 0);
    return (const dnx_data_sch_interface_shaper_resolution_t *) data;

}


shr_error_e
dnx_data_sch_interface_shaper_resolution_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_interface_shaper_resolution_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_table_shaper_resolution);
    data = (const dnx_data_sch_interface_shaper_resolution_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->resolution);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_sch_interface_shaper_resolution_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_interface, dnx_data_sch_interface_table_shaper_resolution);

}






static shr_error_e
dnx_data_sch_device_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "device";
    submodule_data->doc = "DRM information";
    
    submodule_data->nof_features = _dnx_data_sch_device_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch device features");

    submodule_data->features[dnx_data_sch_device_shared_drm].name = "shared_drm";
    submodule_data->features[dnx_data_sch_device_shared_drm].doc = "is shared DRM supported";
    submodule_data->features[dnx_data_sch_device_shared_drm].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sch_device_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch device defines");

    submodule_data->defines[dnx_data_sch_device_define_drm_resolution].name = "drm_resolution";
    submodule_data->defines[dnx_data_sch_device_define_drm_resolution].doc = "resolution of DRM rate";
    
    submodule_data->defines[dnx_data_sch_device_define_drm_resolution].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_sch_device_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch device tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_device_feature_get(
    int unit,
    dnx_data_sch_device_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_device, feature);
}


uint32
dnx_data_sch_device_drm_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_device, dnx_data_sch_device_define_drm_resolution);
}










static shr_error_e
dnx_data_sch_sch_alloc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sch_alloc";
    submodule_data->doc = "e2e scheduler allocation defines";
    
    submodule_data->nof_features = _dnx_data_sch_sch_alloc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch sch_alloc features");

    
    submodule_data->nof_defines = _dnx_data_sch_sch_alloc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch sch_alloc defines");

    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_2].name = "tag_aggregate_se_2";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_2].doc = "2 SE aggregate tag";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_4].name = "tag_aggregate_se_4";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_4].doc = "4 SE aggregate tag";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_4].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_8].name = "tag_aggregate_se_8";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_8].doc = "8 SE aggregate tag";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_aggregate_se_8].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value].name = "shared_shaper_max_tag_value";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value].doc = "max tag value for shared shaper";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_aggregate_se].name = "tag_size_aggregate_se";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_aggregate_se].doc = "tag size for SE aggregate";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_aggregate_se].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_con].name = "tag_size_con";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_con].doc = "tag size for connectors - modid";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_tag_size_con].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_start].name = "type_con_reg_start";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_start].doc = "start of the connectors only region";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_start].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_alloc_invalid_flow].name = "alloc_invalid_flow";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_alloc_invalid_flow].doc = "invalid flow define";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_alloc_invalid_flow].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_dealloc_flow_ids].name = "dealloc_flow_ids";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_dealloc_flow_ids].doc = "max number of flows to deallocate";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_dealloc_flow_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_end].name = "type_con_reg_end";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_end].doc = "end of the connectors only region";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_con_reg_end].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_hr_reg_start].name = "type_hr_reg_start";
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_hr_reg_start].doc = "searching start of the HR region";
    
    submodule_data->defines[dnx_data_sch_sch_alloc_define_type_hr_reg_start].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_sch_alloc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch sch_alloc tables");

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].name = "region";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].doc = "content of quartet in each region type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].size_of_values = sizeof(dnx_data_sch_sch_alloc_region_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].entry_get = dnx_data_sch_sch_alloc_region_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].keys[0].name = "region_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].keys[0].doc = "region type";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_region].nof_values, "_dnx_data_sch_sch_alloc_table_region table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[0].name = "flow_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[0].type = "dnx_sch_element_se_type_e[4]";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[0].doc = "list of elements in quartet";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_region_t, flow_type);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[1].name = "odd_even_mode";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[1].type = "dnx_scheduler_region_odd_even_mode_t";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[1].doc = "Odd/even mode";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_region].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_region_t, odd_even_mode);

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].name = "connector";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].doc = "allocation info for VOQ connectors";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].size_of_values = sizeof(dnx_data_sch_sch_alloc_connector_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].entry_get = dnx_data_sch_sch_alloc_connector_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].keys[0].name = "composite";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].keys[0].doc = "is composite connector";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].keys[1].name = "interdigitated";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].keys[1].doc = "is interdigitated";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].nof_values, "_dnx_data_sch_sch_alloc_table_connector table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[0].name = "alignment";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[0].doc = "alignment requirement";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_connector_t, alignment);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[1].name = "pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[1].doc = "allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_connector_t, pattern);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[2].name = "pattern_size";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[2].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[2].doc = "size of allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[2].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_connector_t, pattern_size);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[3].name = "nof_in_pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[3].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[3].doc = "number of elements in allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_connector].values[3].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_connector_t, nof_in_pattern);

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].name = "se_per_region_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].doc = "allocation info for scheduling elements per region type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].size_of_values = sizeof(dnx_data_sch_sch_alloc_se_per_region_type_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].entry_get = dnx_data_sch_sch_alloc_se_per_region_type_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].keys[0].name = "flow_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].keys[0].doc = "type of element";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].keys[1].name = "region_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].keys[1].doc = "type of region";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].nof_values, "_dnx_data_sch_sch_alloc_table_se_per_region_type table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[0].name = "valid";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[0].doc = "is the entry valid. In case the entry is not valid, the data is taken from se table";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, valid);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[1].name = "alignment";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[1].doc = "alignment requirement";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, alignment);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[2].name = "pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[2].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[2].doc = "allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[2].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, pattern);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[3].name = "pattern_size";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[3].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[3].doc = "size of allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[3].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, pattern_size);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[4].name = "nof_offsets";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[4].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[4].doc = "size offset array";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[4].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, nof_offsets);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[5].name = "offset";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[5].type = "uint32[4]";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[5].doc = "list of offsets";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se_per_region_type].values[5].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_per_region_type_t, offset);

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].name = "se";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].doc = "allocation info for scheduling elements";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].size_of_values = sizeof(dnx_data_sch_sch_alloc_se_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].entry_get = dnx_data_sch_sch_alloc_se_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].nof_keys = 1;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].keys[0].name = "flow_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].keys[0].doc = "type of element";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_se].nof_values, "_dnx_data_sch_sch_alloc_table_se table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[0].name = "alignment";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[0].doc = "alignment requirement";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_t, alignment);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[1].name = "pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[1].doc = "allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_t, pattern);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[2].name = "pattern_size";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[2].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[2].doc = "size of allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[2].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_t, pattern_size);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[3].name = "nof_offsets";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[3].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[3].doc = "size offset array";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[3].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_t, nof_offsets);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[4].name = "offset";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[4].type = "uint32[4]";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[4].doc = "list of offsets";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_se].values[4].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_se_t, offset);

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].name = "composite_se_per_region_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].doc = "allocation info for composite scheduling elements per region type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].size_of_values = sizeof(dnx_data_sch_sch_alloc_composite_se_per_region_type_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].entry_get = dnx_data_sch_sch_alloc_composite_se_per_region_type_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].keys[0].name = "flow_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].keys[0].doc = "type of element";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].keys[1].name = "region_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].keys[1].doc = "type of region";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].nof_values, "_dnx_data_sch_sch_alloc_table_composite_se_per_region_type table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[0].name = "valid";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[0].doc = "is the entry valid. In case the entry is not valid, the data is taken from composite_se table";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_per_region_type_t, valid);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[1].name = "alignment";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[1].doc = "alignment requirement";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_per_region_type_t, alignment);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[2].name = "pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[2].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[2].doc = "allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[2].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_per_region_type_t, pattern);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[3].name = "pattern_size";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[3].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[3].doc = "size of allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se_per_region_type].values[3].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_per_region_type_t, pattern_size);

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].name = "composite_se";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].doc = "allocation info for composite scheduling elements";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].size_of_values = sizeof(dnx_data_sch_sch_alloc_composite_se_t);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].entry_get = dnx_data_sch_sch_alloc_composite_se_entry_str_get;

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].nof_keys = 2;
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].keys[0].name = "flow_type";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].keys[0].doc = "type of element";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].keys[1].name = "odd_even_mode";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].keys[1].doc = "odd or even region";

    
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].nof_values, "_dnx_data_sch_sch_alloc_table_composite_se table values");
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[0].name = "alignment";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[0].doc = "alignment requirement";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[0].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_t, alignment);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[1].name = "pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[1].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[1].doc = "allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[1].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_t, pattern);
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[2].name = "pattern_size";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[2].type = "uint32";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[2].doc = "size of allocation pattern";
    submodule_data->tables[dnx_data_sch_sch_alloc_table_composite_se].values[2].offset = UTILEX_OFFSETOF(dnx_data_sch_sch_alloc_composite_se_t, pattern_size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_sch_alloc_feature_get(
    int unit,
    dnx_data_sch_sch_alloc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, feature);
}


uint32
dnx_data_sch_sch_alloc_tag_aggregate_se_2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_tag_aggregate_se_2);
}

uint32
dnx_data_sch_sch_alloc_tag_aggregate_se_4_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_tag_aggregate_se_4);
}

uint32
dnx_data_sch_sch_alloc_tag_aggregate_se_8_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_tag_aggregate_se_8);
}

uint32
dnx_data_sch_sch_alloc_shared_shaper_max_tag_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_shared_shaper_max_tag_value);
}

uint32
dnx_data_sch_sch_alloc_tag_size_aggregate_se_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_tag_size_aggregate_se);
}

uint32
dnx_data_sch_sch_alloc_tag_size_con_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_tag_size_con);
}

uint32
dnx_data_sch_sch_alloc_type_con_reg_start_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_type_con_reg_start);
}

uint32
dnx_data_sch_sch_alloc_alloc_invalid_flow_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_alloc_invalid_flow);
}

uint32
dnx_data_sch_sch_alloc_dealloc_flow_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_dealloc_flow_ids);
}

uint32
dnx_data_sch_sch_alloc_type_con_reg_end_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_type_con_reg_end);
}

uint32
dnx_data_sch_sch_alloc_type_hr_reg_start_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_define_type_hr_reg_start);
}



const dnx_data_sch_sch_alloc_region_t *
dnx_data_sch_sch_alloc_region_get(
    int unit,
    int region_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_region);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, region_type, 0);
    return (const dnx_data_sch_sch_alloc_region_t *) data;

}

const dnx_data_sch_sch_alloc_connector_t *
dnx_data_sch_sch_alloc_connector_get(
    int unit,
    int composite,
    int interdigitated)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_connector);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, composite, interdigitated);
    return (const dnx_data_sch_sch_alloc_connector_t *) data;

}

const dnx_data_sch_sch_alloc_se_per_region_type_t *
dnx_data_sch_sch_alloc_se_per_region_type_get(
    int unit,
    int flow_type,
    int region_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se_per_region_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, flow_type, region_type);
    return (const dnx_data_sch_sch_alloc_se_per_region_type_t *) data;

}

const dnx_data_sch_sch_alloc_se_t *
dnx_data_sch_sch_alloc_se_get(
    int unit,
    int flow_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, flow_type, 0);
    return (const dnx_data_sch_sch_alloc_se_t *) data;

}

const dnx_data_sch_sch_alloc_composite_se_per_region_type_t *
dnx_data_sch_sch_alloc_composite_se_per_region_type_get(
    int unit,
    int flow_type,
    int region_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se_per_region_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, flow_type, region_type);
    return (const dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) data;

}

const dnx_data_sch_sch_alloc_composite_se_t *
dnx_data_sch_sch_alloc_composite_se_get(
    int unit,
    int flow_type,
    int odd_even_mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, flow_type, odd_even_mode);
    return (const dnx_data_sch_sch_alloc_composite_se_t *) data;

}


shr_error_e
dnx_data_sch_sch_alloc_region_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_region_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_region);
    data = (const dnx_data_sch_sch_alloc_region_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, 4, data->flow_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->odd_even_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_sch_alloc_connector_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_connector_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_connector);
    data = (const dnx_data_sch_sch_alloc_connector_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->alignment);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_in_pattern);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_sch_alloc_se_per_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_se_per_region_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se_per_region_type);
    data = (const dnx_data_sch_sch_alloc_se_per_region_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->alignment);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern_size);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_offsets);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, 4, data->offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_sch_alloc_se_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_se_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se);
    data = (const dnx_data_sch_sch_alloc_se_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->alignment);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern_size);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_offsets);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, 4, data->offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_sch_alloc_composite_se_per_region_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_composite_se_per_region_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se_per_region_type);
    data = (const dnx_data_sch_sch_alloc_composite_se_per_region_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->alignment);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_sch_sch_alloc_composite_se_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sch_sch_alloc_composite_se_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se);
    data = (const dnx_data_sch_sch_alloc_composite_se_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->alignment);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pattern_size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_region_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_region);

}

const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_connector_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_connector);

}

const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_se_per_region_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se_per_region_type);

}

const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_se_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_se);

}

const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_composite_se_per_region_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se_per_region_type);

}

const dnxc_data_table_info_t *
dnx_data_sch_sch_alloc_composite_se_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_sch_alloc, dnx_data_sch_sch_alloc_table_composite_se);

}






static shr_error_e
dnx_data_sch_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "e2e scheduler dbal defines";
    
    submodule_data->nof_features = _dnx_data_sch_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch dbal features");

    
    submodule_data->nof_defines = _dnx_data_sch_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch dbal defines");

    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_grouping_factor].name = "scheduler_enable_grouping_factor";
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_grouping_factor].doc = "size of group of sched elements sharing the same HW field ";
    
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_grouping_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor].name = "scheduler_enable_dbal_mult_factor";
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor].doc = "DBAL multiplication factor";
    
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_flow_bits].name = "flow_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_flow_bits].doc = "Number of bits in flow id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_flow_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_se_bits].name = "se_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_se_bits].doc = "Number of bits in Scheduling element id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_se_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_cl_bits].name = "cl_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_cl_bits].doc = "Number of bits in CL id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_cl_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_hr_bits].name = "hr_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_hr_bits].doc = "Number of bits in HR id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_hr_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_interface_bits].name = "interface_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_interface_bits].doc = "Number of bits in interface id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_interface_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_calendar_bits].name = "calendar_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_calendar_bits].doc = "Number of bits in calendar id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_calendar_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_mant_bits].name = "flow_shaper_mant_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_mant_bits].doc = "Number of bits in flow shaper mantissa";
    
    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_mant_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_quanta_bits].name = "ps_shaper_quanta_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_quanta_bits].doc = "number of bits in PS shaper quanta";
    
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_quanta_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_max_burst_bits].name = "ps_shaper_max_burst_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_max_burst_bits].doc = "number of bits in PS shaper max burst";
    
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_max_burst_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_drm_nof_links_max].name = "drm_nof_links_max";
    submodule_data->defines[dnx_data_sch_dbal_define_drm_nof_links_max].doc = "maximal value of number of links for DRM table";
    
    submodule_data->defines[dnx_data_sch_dbal_define_drm_nof_links_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_key_size].name = "scheduler_enable_key_size";
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_key_size].doc = "size in bits of key in scheduler enable DBAL table";
    
    submodule_data->defines[dnx_data_sch_dbal_define_scheduler_enable_key_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_key_size].name = "flow_id_pair_key_size";
    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_key_size].doc = "size in bits of half flow id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_key_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_max].name = "flow_id_pair_max";
    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_max].doc = "max value of half flow id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_flow_id_pair_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_ps_bits].name = "ps_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_ps_bits].doc = "Number of bits in PS id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_ps_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_descr_bits].name = "flow_shaper_descr_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_descr_bits].doc = "Number of bits in flow shaper descriptor for one flow";
    
    submodule_data->defines[dnx_data_sch_dbal_define_flow_shaper_descr_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_bits].name = "ps_shaper_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_bits].doc = "number of bits in PS shaper";
    
    submodule_data->defines[dnx_data_sch_dbal_define_ps_shaper_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sch_dbal_define_cl_profile_bits].name = "cl_profile_bits";
    submodule_data->defines[dnx_data_sch_dbal_define_cl_profile_bits].doc = "number of bits in CL profile id";
    
    submodule_data->defines[dnx_data_sch_dbal_define_cl_profile_bits].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sch_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_dbal_feature_get(
    int unit,
    dnx_data_sch_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, feature);
}


uint32
dnx_data_sch_dbal_scheduler_enable_grouping_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_scheduler_enable_grouping_factor);
}

uint32
dnx_data_sch_dbal_scheduler_enable_dbal_mult_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_scheduler_enable_dbal_mult_factor);
}

uint32
dnx_data_sch_dbal_flow_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_flow_bits);
}

uint32
dnx_data_sch_dbal_se_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_se_bits);
}

uint32
dnx_data_sch_dbal_cl_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_cl_bits);
}

uint32
dnx_data_sch_dbal_hr_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_hr_bits);
}

uint32
dnx_data_sch_dbal_interface_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_interface_bits);
}

uint32
dnx_data_sch_dbal_calendar_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_calendar_bits);
}

uint32
dnx_data_sch_dbal_flow_shaper_mant_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_flow_shaper_mant_bits);
}

uint32
dnx_data_sch_dbal_ps_shaper_quanta_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_ps_shaper_quanta_bits);
}

uint32
dnx_data_sch_dbal_ps_shaper_max_burst_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_ps_shaper_max_burst_bits);
}

uint32
dnx_data_sch_dbal_drm_nof_links_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_drm_nof_links_max);
}

uint32
dnx_data_sch_dbal_scheduler_enable_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_scheduler_enable_key_size);
}

uint32
dnx_data_sch_dbal_flow_id_pair_key_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_flow_id_pair_key_size);
}

uint32
dnx_data_sch_dbal_flow_id_pair_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_flow_id_pair_max);
}

uint32
dnx_data_sch_dbal_ps_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_ps_bits);
}

uint32
dnx_data_sch_dbal_flow_shaper_descr_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_flow_shaper_descr_bits);
}

uint32
dnx_data_sch_dbal_ps_shaper_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_ps_shaper_bits);
}

uint32
dnx_data_sch_dbal_cl_profile_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_dbal, dnx_data_sch_dbal_define_cl_profile_bits);
}










static shr_error_e
dnx_data_sch_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "Per devices features";
    
    submodule_data->nof_features = _dnx_data_sch_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sch features features");

    submodule_data->features[dnx_data_sch_features_dlm].name = "dlm";
    submodule_data->features[dnx_data_sch_features_dlm].doc = "";
    submodule_data->features[dnx_data_sch_features_dlm].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sch_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sch features defines");

    
    submodule_data->nof_tables = _dnx_data_sch_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sch features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sch_features_feature_get(
    int unit,
    dnx_data_sch_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sch, dnx_data_sch_submodule_features, feature);
}








shr_error_e
dnx_data_sch_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "sch";
    module_data->nof_submodules = _dnx_data_sch_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data sch submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_sch_general_init(unit, &module_data->submodules[dnx_data_sch_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_sch_ps_init(unit, &module_data->submodules[dnx_data_sch_submodule_ps]));
    SHR_IF_ERR_EXIT(dnx_data_sch_flow_init(unit, &module_data->submodules[dnx_data_sch_submodule_flow]));
    SHR_IF_ERR_EXIT(dnx_data_sch_se_init(unit, &module_data->submodules[dnx_data_sch_submodule_se]));
    SHR_IF_ERR_EXIT(dnx_data_sch_interface_init(unit, &module_data->submodules[dnx_data_sch_submodule_interface]));
    SHR_IF_ERR_EXIT(dnx_data_sch_device_init(unit, &module_data->submodules[dnx_data_sch_submodule_device]));
    SHR_IF_ERR_EXIT(dnx_data_sch_sch_alloc_init(unit, &module_data->submodules[dnx_data_sch_submodule_sch_alloc]));
    SHR_IF_ERR_EXIT(dnx_data_sch_dbal_init(unit, &module_data->submodules[dnx_data_sch_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_sch_features_init(unit, &module_data->submodules[dnx_data_sch_submodule_features]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sch_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sch_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_sch_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

