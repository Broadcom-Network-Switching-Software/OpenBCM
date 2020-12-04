

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_graphical.h>



extern shr_error_e jr2_a0_data_graphical_attach(
    int unit);
extern shr_error_e j2c_a0_data_graphical_attach(
    int unit);
extern shr_error_e q2a_a0_data_graphical_attach(
    int unit);
extern shr_error_e j2p_a0_data_graphical_attach(
    int unit);



static shr_error_e
dnx_data_graphical_diag_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "diag";
    submodule_data->doc = "diag counter graphical.";
    
    submodule_data->nof_features = _dnx_data_graphical_diag_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data graphical diag features");

    
    submodule_data->nof_defines = _dnx_data_graphical_diag_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data graphical diag defines");

    submodule_data->defines[dnx_data_graphical_diag_define_num_of_rows].name = "num_of_rows";
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_rows].doc = "rows of diag cou graphic command";
    
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_rows].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_graphical_diag_define_num_of_index].name = "num_of_index";
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_index].doc = "nunber of index per block";
    
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_index].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_graphical_diag_define_num_of_block_entries].name = "num_of_block_entries";
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_block_entries].doc = "number of entries per block";
    
    submodule_data->defines[dnx_data_graphical_diag_define_num_of_block_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_graphical_diag_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data graphical diag tables");

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].name = "blocks_left";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].doc = "graphical table blocks information on left side(in)";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].size_of_values = sizeof(dnx_data_graphical_diag_blocks_left_t);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].entry_get = dnx_data_graphical_diag_blocks_left_entry_str_get;

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].nof_keys = 2;
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].keys[0].name = "row";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].keys[0].doc = "graphic display row";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].keys[1].name = "index";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].keys[1].doc = "index";

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values, dnxc_data_value_t, submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].nof_values, "_dnx_data_graphical_diag_table_blocks_left table values");
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[0].name = "block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[0].type = "dnx_diag_graphical_block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[0].doc = "diag display block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[0].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_left_t, block);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[1].name = "name";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[1].type = "char *";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[1].doc = "block name";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[1].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_left_t, name);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[2].name = "valid";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[2].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[2].doc = "block is valid";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[2].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_left_t, valid);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[3].name = "per_core";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[3].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[3].doc = "support multi-cores";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_left].values[3].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_left_t, per_core);

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].name = "blocks_right";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].doc = "graphical table blocks information on right side(out)";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].size_of_values = sizeof(dnx_data_graphical_diag_blocks_right_t);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].entry_get = dnx_data_graphical_diag_blocks_right_entry_str_get;

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].nof_keys = 2;
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].keys[0].name = "row";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].keys[0].doc = "graphic display row";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].keys[1].name = "index";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].keys[1].doc = "index";

    
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values, dnxc_data_value_t, submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].nof_values, "_dnx_data_graphical_diag_table_blocks_right table values");
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[0].name = "block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[0].type = "dnx_diag_graphical_block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[0].doc = "diag display block";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[0].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_right_t, block);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[1].name = "name";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[1].type = "char *";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[1].doc = "block name";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[1].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_right_t, name);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[2].name = "valid";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[2].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[2].doc = "block is valid";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[2].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_right_t, valid);
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[3].name = "per_core";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[3].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[3].doc = "support multi-cores";
    submodule_data->tables[dnx_data_graphical_diag_table_blocks_right].values[3].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_blocks_right_t, per_core);

    
    submodule_data->tables[dnx_data_graphical_diag_table_counters].name = "counters";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].doc = "counter information";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_graphical_diag_table_counters].size_of_values = sizeof(dnx_data_graphical_diag_counters_t);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].entry_get = dnx_data_graphical_diag_counters_entry_str_get;

    
    submodule_data->tables[dnx_data_graphical_diag_table_counters].nof_keys = 2;
    submodule_data->tables[dnx_data_graphical_diag_table_counters].keys[0].name = "block";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].keys[0].doc = "block information";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].keys[1].name = "index";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].keys[1].doc = "index in block";

    
    submodule_data->tables[dnx_data_graphical_diag_table_counters].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_graphical_diag_table_counters].values, dnxc_data_value_t, submodule_data->tables[dnx_data_graphical_diag_table_counters].nof_values, "_dnx_data_graphical_diag_table_counters table values");
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[0].name = "name";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[0].type = "char *";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[0].doc = "counter name";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[0].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, name);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[1].name = "short_name";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[1].type = "char *";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[1].doc = "abbreviation name";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[1].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, short_name);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[2].name = "regs";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[2].type = "int[DNX_DIAG_COUNTER_REG_NOF_REGS]";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[2].doc = "register name for counter";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[2].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, regs);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[3].name = "flds";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[3].type = "int[DNX_DIAG_COUNTER_REG_NOF_REGS]";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[3].doc = "field name for counter";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[3].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, flds);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[4].name = "overflow_field";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[4].type = "int[DNX_DIAG_COUNTER_REG_NOF_REGS]";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[4].doc = "overflow field name for counter";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[4].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, overflow_field);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[5].name = "arr_i";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[5].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[5].doc = "attribution index";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[5].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, arr_i);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[6].name = "fld_num";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[6].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[6].doc = "counter field number";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[6].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, fld_num);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[7].name = "doc";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[7].type = "char *";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[7].doc = "counter description";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[7].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, doc);
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[8].name = "is_err";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[8].type = "uint32";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[8].doc = "error information need mask red";
    submodule_data->tables[dnx_data_graphical_diag_table_counters].values[8].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_counters_t, is_err);

    
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].name = "drop_reason";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].doc = "drop reason information";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].size_of_values = sizeof(dnx_data_graphical_diag_drop_reason_t);
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].entry_get = dnx_data_graphical_diag_drop_reason_entry_str_get;

    
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].nof_keys = 1;
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].keys[0].name = "index";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].keys[0].doc = "index in block";

    
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values, dnxc_data_value_t, submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].nof_values, "_dnx_data_graphical_diag_table_drop_reason table values");
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[0].name = "regs";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[0].doc = "register name for counter";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[0].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_drop_reason_t, regs);
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[1].name = "flds";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[1].doc = "field name for counter";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[1].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_drop_reason_t, flds);
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[2].name = "drop_reason_cb";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[2].type = "dnx_dropped_reason_get_f";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[2].doc = "callback to get drop reason";
    submodule_data->tables[dnx_data_graphical_diag_table_drop_reason].values[2].offset = UTILEX_OFFSETOF(dnx_data_graphical_diag_drop_reason_t, drop_reason_cb);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_graphical_diag_feature_get(
    int unit,
    dnx_data_graphical_diag_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, feature);
}


uint32
dnx_data_graphical_diag_num_of_rows_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_define_num_of_rows);
}

uint32
dnx_data_graphical_diag_num_of_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_define_num_of_index);
}

uint32
dnx_data_graphical_diag_num_of_block_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_define_num_of_block_entries);
}



const dnx_data_graphical_diag_blocks_left_t *
dnx_data_graphical_diag_blocks_left_get(
    int unit,
    int row,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_left);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, row, index);
    return (const dnx_data_graphical_diag_blocks_left_t *) data;

}

const dnx_data_graphical_diag_blocks_right_t *
dnx_data_graphical_diag_blocks_right_get(
    int unit,
    int row,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_right);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, row, index);
    return (const dnx_data_graphical_diag_blocks_right_t *) data;

}

const dnx_data_graphical_diag_counters_t *
dnx_data_graphical_diag_counters_get(
    int unit,
    int block,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_counters);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, index);
    return (const dnx_data_graphical_diag_counters_t *) data;

}

const dnx_data_graphical_diag_drop_reason_t *
dnx_data_graphical_diag_drop_reason_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_drop_reason);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_graphical_diag_drop_reason_t *) data;

}


shr_error_e
dnx_data_graphical_diag_blocks_left_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_graphical_diag_blocks_left_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_left);
    data = (const dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->per_core);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_graphical_diag_blocks_right_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_graphical_diag_blocks_right_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_right);
    data = (const dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->per_core);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_graphical_diag_counters_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_graphical_diag_counters_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_counters);
    data = (const dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->short_name == NULL ? "" : data->short_name);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DIAG_COUNTER_REG_NOF_REGS, data->regs);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DIAG_COUNTER_REG_NOF_REGS, data->flds);
            break;
        case 4:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DIAG_COUNTER_REG_NOF_REGS, data->overflow_field);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->arr_i);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fld_num);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_err);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_graphical_diag_drop_reason_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_graphical_diag_drop_reason_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_drop_reason);
    data = (const dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->regs);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flds);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->drop_reason_cb == NULL ? "NULL" : "func");
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_graphical_diag_blocks_left_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_left);

}

const dnxc_data_table_info_t *
dnx_data_graphical_diag_blocks_right_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_blocks_right);

}

const dnxc_data_table_info_t *
dnx_data_graphical_diag_counters_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_counters);

}

const dnxc_data_table_info_t *
dnx_data_graphical_diag_drop_reason_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_graphical, dnx_data_graphical_submodule_diag, dnx_data_graphical_diag_table_drop_reason);

}



shr_error_e
dnx_data_graphical_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "graphical";
    module_data->nof_submodules = _dnx_data_graphical_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data graphical submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_graphical_diag_init(unit, &module_data->submodules[dnx_data_graphical_submodule_diag]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_graphical_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_graphical_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_graphical_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

