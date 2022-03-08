
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_debug.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_debug_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_debug_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_debug_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_debug_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_debug_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_debug_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_debug_mem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mem";
    submodule_data->doc = "Inforamtion per debug memory block";
    
    submodule_data->nof_features = _dnx_data_debug_mem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data debug mem features");

    
    submodule_data->nof_defines = _dnx_data_debug_mem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data debug mem defines");

    
    submodule_data->nof_tables = _dnx_data_debug_mem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data debug mem tables");

    
    submodule_data->tables[dnx_data_debug_mem_table_params].name = "params";
    submodule_data->tables[dnx_data_debug_mem_table_params].doc = "Per ASIC block properties and characteristics";
    submodule_data->tables[dnx_data_debug_mem_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_debug_mem_table_params].size_of_values = sizeof(dnx_data_debug_mem_params_t);
    submodule_data->tables[dnx_data_debug_mem_table_params].entry_get = dnx_data_debug_mem_params_entry_str_get;

    
    submodule_data->tables[dnx_data_debug_mem_table_params].nof_keys = 1;
    submodule_data->tables[dnx_data_debug_mem_table_params].keys[0].name = "block";
    submodule_data->tables[dnx_data_debug_mem_table_params].keys[0].doc = "ASIC Block ID";

    
    submodule_data->tables[dnx_data_debug_mem_table_params].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_debug_mem_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_debug_mem_table_params].nof_values, "_dnx_data_debug_mem_table_params table values");
    submodule_data->tables[dnx_data_debug_mem_table_params].values[0].name = "valid";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[0].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[0].doc = "marks existence of debug memory for ASIC block";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_params_t, valid);
    submodule_data->tables[dnx_data_debug_mem_table_params].values[1].name = "line_nof";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[1].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[1].doc = "number of lines in debug memory";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_params_t, line_nof);
    submodule_data->tables[dnx_data_debug_mem_table_params].values[2].name = "width";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[2].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[2].doc = "memory width - size of lingest line";
    submodule_data->tables[dnx_data_debug_mem_table_params].values[2].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_params_t, width);

    
    submodule_data->tables[dnx_data_debug_mem_table_block_map].name = "block_map";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].doc = "Per ASIC block visibility info";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_debug_mem_table_block_map].size_of_values = sizeof(dnx_data_debug_mem_block_map_t);
    submodule_data->tables[dnx_data_debug_mem_table_block_map].entry_get = dnx_data_debug_mem_block_map_entry_str_get;

    
    submodule_data->tables[dnx_data_debug_mem_table_block_map].nof_keys = 1;
    submodule_data->tables[dnx_data_debug_mem_table_block_map].keys[0].name = "block";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].keys[0].doc = "ASIC Block ID";

    
    submodule_data->tables[dnx_data_debug_mem_table_block_map].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_debug_mem_table_block_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_debug_mem_table_block_map].nof_values, "_dnx_data_debug_mem_table_block_map table values");
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[0].name = "block_id";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[0].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[0].doc = "block id";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_block_map_t, block_id);
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[1].name = "is_asic_block";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[1].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[1].doc = "Values: No - 0, Yes - 1";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_block_map_t, is_asic_block);
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[2].name = "dbal_table";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[2].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[2].doc = "dbal table";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_block_map_t, dbal_table);
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[3].name = "dbal_field";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[3].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[3].doc = "dbal field";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[3].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_block_map_t, dbal_field);
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[4].name = "direction";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[4].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[4].doc = "direction";
    submodule_data->tables[dnx_data_debug_mem_table_block_map].values[4].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_block_map_t, direction);

    
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].name = "mem_array";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].doc = "Per ASIC block memory register name, mem1 is the name used in some devices and mem2 is the name used in others";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].size_of_values = sizeof(dnx_data_debug_mem_mem_array_t);
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].entry_get = dnx_data_debug_mem_mem_array_entry_str_get;

    
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].nof_keys = 1;
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].keys[0].name = "block";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].keys[0].doc = "ASIC Block ID";

    
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_debug_mem_table_mem_array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_debug_mem_table_mem_array].nof_values, "_dnx_data_debug_mem_table_mem_array table values");
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[0].name = "mem1";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[0].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[0].doc = "memory internal name, example: IPPA_MEM_F000000m";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[0].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_mem_array_t, mem1);
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[1].name = "mem2";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[1].type = "int";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[1].doc = "memory informative name, example: IPPA_IPPA_DEBUG_UNITm";
    submodule_data->tables[dnx_data_debug_mem_table_mem_array].values[1].offset = UTILEX_OFFSETOF(dnx_data_debug_mem_mem_array_t, mem2);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_debug_mem_feature_get(
    int unit,
    dnx_data_debug_mem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, feature);
}




const dnx_data_debug_mem_params_t *
dnx_data_debug_mem_params_get(
    int unit,
    int block)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, 0);
    return (const dnx_data_debug_mem_params_t *) data;

}

const dnx_data_debug_mem_block_map_t *
dnx_data_debug_mem_block_map_get(
    int unit,
    int block)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_block_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, 0);
    return (const dnx_data_debug_mem_block_map_t *) data;

}

const dnx_data_debug_mem_mem_array_t *
dnx_data_debug_mem_mem_array_get(
    int unit,
    int block)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_mem_array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, 0);
    return (const dnx_data_debug_mem_mem_array_t *) data;

}


shr_error_e
dnx_data_debug_mem_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_debug_mem_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_params);
    data = (const dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->line_nof);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->width);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_debug_mem_block_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_debug_mem_block_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_block_map);
    data = (const dnx_data_debug_mem_block_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_asic_block);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_field);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->direction);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_debug_mem_mem_array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_debug_mem_mem_array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_mem_array);
    data = (const dnx_data_debug_mem_mem_array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem1);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem2);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_debug_mem_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_params);

}

const dnxc_data_table_info_t *
dnx_data_debug_mem_block_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_block_map);

}

const dnxc_data_table_info_t *
dnx_data_debug_mem_mem_array_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_mem, dnx_data_debug_mem_table_mem_array);

}






static shr_error_e
dnx_data_debug_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_debug_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data debug feature features");

    submodule_data->features[dnx_data_debug_feature_no_hit_bit_on_mdb_access].name = "no_hit_bit_on_mdb_access";
    submodule_data->features[dnx_data_debug_feature_no_hit_bit_on_mdb_access].doc = "If KBR is assigned to 1st access, hit bit is not relevant";
    submodule_data->features[dnx_data_debug_feature_no_hit_bit_on_mdb_access].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_debug_feature_pem_vt_init_fix].name = "pem_vt_init_fix";
    submodule_data->features[dnx_data_debug_feature_pem_vt_init_fix].doc = "";
    submodule_data->features[dnx_data_debug_feature_pem_vt_init_fix].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_debug_feature_iparser_signals_valid].name = "iparser_signals_valid";
    submodule_data->features[dnx_data_debug_feature_iparser_signals_valid].doc = "If set, indicates that IParser signals are valid";
    submodule_data->features[dnx_data_debug_feature_iparser_signals_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_debug_feature_fwd_action_signals_from_lbp_valid].name = "fwd_action_signals_from_lbp_valid";
    submodule_data->features[dnx_data_debug_feature_fwd_action_signals_from_lbp_valid].doc = "If set, indicates that fwd_action_* signals from LBP stage are valid";
    submodule_data->features[dnx_data_debug_feature_fwd_action_signals_from_lbp_valid].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_debug_feature_esem_result_signals_valid].name = "esem_result_signals_valid";
    submodule_data->features[dnx_data_debug_feature_esem_result_signals_valid].doc = "If set, indicates that esem result signals are valid";
    submodule_data->features[dnx_data_debug_feature_esem_result_signals_valid].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_debug_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data debug feature defines");

    
    submodule_data->nof_tables = _dnx_data_debug_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data debug feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_debug_feature_feature_get(
    int unit,
    dnx_data_debug_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_feature, feature);
}











static shr_error_e
dnx_data_debug_kleap_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "kleap";
    submodule_data->doc = "kleap debug inforamtion";
    
    submodule_data->nof_features = _dnx_data_debug_kleap_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data debug kleap features");

    
    submodule_data->nof_defines = _dnx_data_debug_kleap_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data debug kleap defines");

    submodule_data->defines[dnx_data_debug_kleap_define_fwd1_gen_data_size].name = "fwd1_gen_data_size";
    submodule_data->defines[dnx_data_debug_kleap_define_fwd1_gen_data_size].doc = "FWD1 general data size, used to find the const values";
    
    submodule_data->defines[dnx_data_debug_kleap_define_fwd1_gen_data_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_kleap_define_fwd2_gen_data_size].name = "fwd2_gen_data_size";
    submodule_data->defines[dnx_data_debug_kleap_define_fwd2_gen_data_size].doc = "FWD2 general data size, used to find the const values";
    
    submodule_data->defines[dnx_data_debug_kleap_define_fwd2_gen_data_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_debug_kleap_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data debug kleap tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_debug_kleap_feature_get(
    int unit,
    dnx_data_debug_kleap_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_kleap, feature);
}


uint32
dnx_data_debug_kleap_fwd1_gen_data_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_kleap, dnx_data_debug_kleap_define_fwd1_gen_data_size);
}

uint32
dnx_data_debug_kleap_fwd2_gen_data_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_kleap, dnx_data_debug_kleap_define_fwd2_gen_data_size);
}










static shr_error_e
dnx_data_debug_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general inforamtion";
    
    submodule_data->nof_features = _dnx_data_debug_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data debug general features");

    
    submodule_data->nof_defines = _dnx_data_debug_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data debug general defines");

    submodule_data->defines[dnx_data_debug_general_define_global_visibility].name = "global_visibility";
    submodule_data->defines[dnx_data_debug_general_define_global_visibility].doc = " visibility global enabler";
    
    submodule_data->defines[dnx_data_debug_general_define_global_visibility].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_has_etpp_visibility_enabler].name = "has_etpp_visibility_enabler";
    submodule_data->defines[dnx_data_debug_general_define_has_etpp_visibility_enabler].doc = "Indicates whether etpp visibility is enabled seperatly or includes in irpp enabler";
    
    submodule_data->defines[dnx_data_debug_general_define_has_etpp_visibility_enabler].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_has_periodic_visibility].name = "has_periodic_visibility";
    submodule_data->defines[dnx_data_debug_general_define_has_periodic_visibility].doc = "Indicates whether the device supports pereiodic visibility";
    
    submodule_data->defines[dnx_data_debug_general_define_has_periodic_visibility].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold].name = "etpp_eop_and_sop_dec_above_threshold";
    submodule_data->defines[dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold].doc = "eop/sop decrement above threshold signal";
    
    submodule_data->defines[dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_egress_port_mc_visibility].name = "egress_port_mc_visibility";
    submodule_data->defines[dnx_data_debug_general_define_egress_port_mc_visibility].doc = "port-visibility control at egress MC";
    
    submodule_data->defines[dnx_data_debug_general_define_egress_port_mc_visibility].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_pmf_debug_signals].name = "pmf_debug_signals";
    submodule_data->defines[dnx_data_debug_general_define_pmf_debug_signals].doc = "Indicates whether pmf debug signals are accessible.";
    
    submodule_data->defines[dnx_data_debug_general_define_pmf_debug_signals].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_fer_debug_signals].name = "fer_debug_signals";
    submodule_data->defines[dnx_data_debug_general_define_fer_debug_signals].doc = "Indicates whether fer debug signals are accessible.";
    
    submodule_data->defines[dnx_data_debug_general_define_fer_debug_signals].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_ees_data_device_debug_signals].name = "ees_data_device_debug_signals";
    submodule_data->defines[dnx_data_debug_general_define_ees_data_device_debug_signals].doc = "Indicates whether the EES Data signals come from device or adapter.";
    
    submodule_data->defines[dnx_data_debug_general_define_ees_data_device_debug_signals].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_lbp_debug_signals_register_based].name = "lbp_debug_signals_register_based";
    submodule_data->defines[dnx_data_debug_general_define_lbp_debug_signals_register_based].doc = "Indicates LBP debug signals are register based, LBP signals are always show the last packet when it's register based.";
    
    submodule_data->defines[dnx_data_debug_general_define_lbp_debug_signals_register_based].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_debug_general_define_mem_array_index].name = "mem_array_index";
    submodule_data->defines[dnx_data_debug_general_define_mem_array_index].doc = "used debug mem array index according to device";
    
    submodule_data->defines[dnx_data_debug_general_define_mem_array_index].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_debug_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data debug general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_debug_general_feature_get(
    int unit,
    dnx_data_debug_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, feature);
}


uint32
dnx_data_debug_general_global_visibility_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_global_visibility);
}

uint32
dnx_data_debug_general_has_etpp_visibility_enabler_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_has_etpp_visibility_enabler);
}

uint32
dnx_data_debug_general_has_periodic_visibility_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_has_periodic_visibility);
}

uint32
dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold);
}

uint32
dnx_data_debug_general_egress_port_mc_visibility_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_egress_port_mc_visibility);
}

uint32
dnx_data_debug_general_pmf_debug_signals_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_pmf_debug_signals);
}

uint32
dnx_data_debug_general_fer_debug_signals_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_fer_debug_signals);
}

uint32
dnx_data_debug_general_ees_data_device_debug_signals_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_ees_data_device_debug_signals);
}

uint32
dnx_data_debug_general_lbp_debug_signals_register_based_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_lbp_debug_signals_register_based);
}

uint32
dnx_data_debug_general_mem_array_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_general, dnx_data_debug_general_define_mem_array_index);
}










static shr_error_e
dnx_data_debug_nif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "nif";
    submodule_data->doc = "nif debug inforamtion";
    
    submodule_data->nof_features = _dnx_data_debug_nif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data debug nif features");

    
    submodule_data->nof_defines = _dnx_data_debug_nif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data debug nif defines");

    submodule_data->defines[dnx_data_debug_nif_define_extension_header].name = "extension_header";
    submodule_data->defines[dnx_data_debug_nif_define_extension_header].doc = "Presenting the device has extension 240B header in NIF stage";
    
    submodule_data->defines[dnx_data_debug_nif_define_extension_header].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_debug_nif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data debug nif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_debug_nif_feature_get(
    int unit,
    dnx_data_debug_nif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_nif, feature);
}


uint32
dnx_data_debug_nif_extension_header_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_debug, dnx_data_debug_submodule_nif, dnx_data_debug_nif_define_extension_header);
}







shr_error_e
dnx_data_debug_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "debug";
    module_data->nof_submodules = _dnx_data_debug_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data debug submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_debug_mem_init(unit, &module_data->submodules[dnx_data_debug_submodule_mem]));
    SHR_IF_ERR_EXIT(dnx_data_debug_feature_init(unit, &module_data->submodules[dnx_data_debug_submodule_feature]));
    SHR_IF_ERR_EXIT(dnx_data_debug_kleap_init(unit, &module_data->submodules[dnx_data_debug_submodule_kleap]));
    SHR_IF_ERR_EXIT(dnx_data_debug_general_init(unit, &module_data->submodules[dnx_data_debug_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_debug_nif_init(unit, &module_data->submodules[dnx_data_debug_submodule_nif]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_debug_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_debug_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_debug_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

