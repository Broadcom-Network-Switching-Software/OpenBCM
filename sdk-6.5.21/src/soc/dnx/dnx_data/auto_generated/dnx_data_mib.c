

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mib.h>



extern shr_error_e jr2_a0_data_mib_attach(
    int unit);
extern shr_error_e j2p_a0_data_mib_attach(
    int unit);



static shr_error_e
dnx_data_mib_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general mib proprties and data";
    
    submodule_data->nof_features = _dnx_data_mib_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mib general features");

    submodule_data->features[dnx_data_mib_general_mib_counters_support].name = "mib_counters_support";
    submodule_data->features[dnx_data_mib_general_mib_counters_support].doc = "Indicate if mib counters are supported";
    submodule_data->features[dnx_data_mib_general_mib_counters_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mib_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mib general defines");

    submodule_data->defines[dnx_data_mib_general_define_nof_row_per_cdu_lane].name = "nof_row_per_cdu_lane";
    submodule_data->defines[dnx_data_mib_general_define_nof_row_per_cdu_lane].doc = "Indicate number of row used per cdu lane.";
    
    submodule_data->defines[dnx_data_mib_general_define_nof_row_per_cdu_lane].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mib_general_define_stat_interval].name = "stat_interval";
    submodule_data->defines[dnx_data_mib_general_define_stat_interval].doc = "Set statistics collection interval in microseconds";
    
    submodule_data->defines[dnx_data_mib_general_define_stat_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mib_general_define_stat_jumbo].name = "stat_jumbo";
    submodule_data->defines[dnx_data_mib_general_define_stat_jumbo].doc = "Maximum packet size used in statistics counter update";
    
    submodule_data->defines[dnx_data_mib_general_define_stat_jumbo].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mib_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mib general tables");

    
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].name = "stat_pbmp";
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].doc = "Bitmap of ports to enable statistic counter collection";
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].size_of_values = sizeof(dnx_data_mib_general_stat_pbmp_t);
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].entry_get = dnx_data_mib_general_stat_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].nof_keys = 0;

    
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].nof_values, "_dnx_data_mib_general_table_stat_pbmp table values");
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].values[0].name = "pbmp";
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].values[0].doc = "Bitmap of ports to enable statistic counter collection";
    submodule_data->tables[dnx_data_mib_general_table_stat_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_mib_general_stat_pbmp_t, pbmp);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mib_general_feature_get(
    int unit,
    dnx_data_mib_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, feature);
}


uint32
dnx_data_mib_general_nof_row_per_cdu_lane_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_define_nof_row_per_cdu_lane);
}

uint32
dnx_data_mib_general_stat_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_define_stat_interval);
}

uint32
dnx_data_mib_general_stat_jumbo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_define_stat_jumbo);
}



const dnx_data_mib_general_stat_pbmp_t *
dnx_data_mib_general_stat_pbmp_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_table_stat_pbmp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_mib_general_stat_pbmp_t *) data;

}


shr_error_e
dnx_data_mib_general_stat_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mib_general_stat_pbmp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_table_stat_pbmp);
    data = (const dnx_data_mib_general_stat_pbmp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->pbmp);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mib_general_stat_pbmp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_general, dnx_data_mib_general_table_stat_pbmp);

}






static shr_error_e
dnx_data_mib_controlled_counters_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "controlled_counters";
    submodule_data->doc = "Controlled counters data";
    
    submodule_data->nof_features = _dnx_data_mib_controlled_counters_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mib controlled_counters features");

    
    submodule_data->nof_defines = _dnx_data_mib_controlled_counters_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mib controlled_counters defines");

    
    submodule_data->nof_tables = _dnx_data_mib_controlled_counters_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mib controlled_counters tables");

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].name = "map_data";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].doc = "map table of controlled counters - maps bcm counter type and counter set to a set of internal controlled (hw) counters";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].size_of_values = sizeof(dnx_data_mib_controlled_counters_map_data_t);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].entry_get = dnx_data_mib_controlled_counters_map_data_entry_str_get;

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].nof_keys = 2;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].keys[0].name = "counter_type";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].keys[0].doc = "bcm stat id out of bcm_stat_val_t type='bcm_stat_val_t'";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].keys[1].name = "set_type";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].keys[1].doc = "counter set type out of dnx_algo_port_mib_counter_set_type_e";

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].nof_values, "_dnx_data_mib_controlled_counters_table_map_data table values");
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[0].name = "counters_to_add_array";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[0].type = "uint32[DNX_MIB_MAX_COUNTER_BUNDLE]";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[0].doc = "list of counters for addition.";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_map_data_t, counters_to_add_array);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[1].name = "counters_to_subtract_array";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[1].type = "uint32[DNX_MIB_MAX_COUNTER_BUNDLE]";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[1].doc = "list of counters for subtraction";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_map_data_t, counters_to_subtract_array);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[2].name = "counters_to_get_cb";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[2].type = "dnx_mib_counters_map_get_cb_f";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[2].doc = "callback to get the list of required counters";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_map_data].values[2].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_map_data_t, counters_to_get_cb);

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].name = "set_data";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].doc = "counters set info - hw access, documentation, etc...";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].size_of_values = sizeof(dnx_data_mib_controlled_counters_set_data_t);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].entry_get = dnx_data_mib_controlled_counters_set_data_entry_str_get;

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].nof_keys = 2;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].keys[0].name = "counter_id";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].keys[0].doc = "controlled counter id, see - dnx_mib_counter_t'";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].keys[1].name = "set_type";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].keys[1].doc = "counter set type out of dnx_algo_port_mib_counter_set_type_e";

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].nof_values, "_dnx_data_mib_controlled_counters_table_set_data table values");
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[0].name = "hw_counter_id";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[0].type = "int";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[0].doc = "counter id in hw, used for indirect access";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_set_data_t, hw_counter_id);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[1].name = "get_cb";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[1].type = "soc_get_counter_f";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[1].doc = "callback to read the counter value";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_set_data_t, get_cb);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[2].name = "clear_cb";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[2].type = "soc_clear_counter_f";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[2].doc = "callback to clear the counter";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_set_data].values[2].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_set_data_t, clear_cb);

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].name = "counter_data";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].doc = "counter data";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].size_of_values = sizeof(dnx_data_mib_controlled_counters_counter_data_t);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].entry_get = dnx_data_mib_controlled_counters_counter_data_entry_str_get;

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].nof_keys = 1;
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].keys[0].name = "counter_id";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].keys[0].doc = "controlled counter id, see - dnx_mib_counter_t'";

    
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].nof_values, "_dnx_data_mib_controlled_counters_table_counter_data table values");
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[0].name = "length";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[0].type = "int";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[0].doc = "Counter length in bits";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[0].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_counter_data_t, length);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[1].name = "row";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[1].type = "int";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[1].doc = "Which row does counter inside in CDMIB_MEM layout";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[1].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_counter_data_t, row);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[2].name = "offset_in_row";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[2].type = "int";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[2].doc = "Offset in row counter inside CDMIB_MEM layout";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[2].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_counter_data_t, offset_in_row);
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[3].name = "doc";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[3].type = "char *";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[3].doc = "Counter description";
    submodule_data->tables[dnx_data_mib_controlled_counters_table_counter_data].values[3].offset = UTILEX_OFFSETOF(dnx_data_mib_controlled_counters_counter_data_t, doc);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mib_controlled_counters_feature_get(
    int unit,
    dnx_data_mib_controlled_counters_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, feature);
}




const dnx_data_mib_controlled_counters_map_data_t *
dnx_data_mib_controlled_counters_map_data_get(
    int unit,
    int counter_type,
    int set_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_map_data);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, counter_type, set_type);
    return (const dnx_data_mib_controlled_counters_map_data_t *) data;

}

const dnx_data_mib_controlled_counters_set_data_t *
dnx_data_mib_controlled_counters_set_data_get(
    int unit,
    int counter_id,
    int set_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_set_data);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, counter_id, set_type);
    return (const dnx_data_mib_controlled_counters_set_data_t *) data;

}

const dnx_data_mib_controlled_counters_counter_data_t *
dnx_data_mib_controlled_counters_counter_data_get(
    int unit,
    int counter_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_counter_data);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, counter_id, 0);
    return (const dnx_data_mib_controlled_counters_counter_data_t *) data;

}


shr_error_e
dnx_data_mib_controlled_counters_map_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mib_controlled_counters_map_data_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_map_data);
    data = (const dnx_data_mib_controlled_counters_map_data_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_MIB_MAX_COUNTER_BUNDLE, data->counters_to_add_array);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_MIB_MAX_COUNTER_BUNDLE, data->counters_to_subtract_array);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->counters_to_get_cb == NULL ? "NULL" : "func");
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mib_controlled_counters_set_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mib_controlled_counters_set_data_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_set_data);
    data = (const dnx_data_mib_controlled_counters_set_data_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->hw_counter_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->get_cb == NULL ? "NULL" : "func");
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->clear_cb == NULL ? "NULL" : "func");
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_mib_controlled_counters_counter_data_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mib_controlled_counters_counter_data_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_counter_data);
    data = (const dnx_data_mib_controlled_counters_counter_data_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->length);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->row);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset_in_row);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mib_controlled_counters_map_data_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_map_data);

}

const dnxc_data_table_info_t *
dnx_data_mib_controlled_counters_set_data_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_set_data);

}

const dnxc_data_table_info_t *
dnx_data_mib_controlled_counters_counter_data_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mib, dnx_data_mib_submodule_controlled_counters, dnx_data_mib_controlled_counters_table_counter_data);

}



shr_error_e
dnx_data_mib_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "mib";
    module_data->nof_submodules = _dnx_data_mib_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data mib submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_mib_general_init(unit, &module_data->submodules[dnx_data_mib_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_mib_controlled_counters_init(unit, &module_data->submodules[dnx_data_mib_submodule_controlled_counters]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mib_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mib_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

