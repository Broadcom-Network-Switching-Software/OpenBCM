
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_techsupport.h>




extern shr_error_e jr2_a0_data_techsupport_attach(
    int unit);


extern shr_error_e jr2_b0_data_techsupport_attach(
    int unit);


extern shr_error_e j2c_a0_data_techsupport_attach(
    int unit);


extern shr_error_e q2a_a0_data_techsupport_attach(
    int unit);


extern shr_error_e j2p_a0_data_techsupport_attach(
    int unit);




static shr_error_e
dnx_data_techsupport_tm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_techsupport_tm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data techsupport tm features");

    
    submodule_data->nof_defines = _dnx_data_techsupport_tm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data techsupport tm defines");

    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tables_per_block].name = "num_of_tables_per_block";
    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tables_per_block].doc = "";
    
    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tables_per_block].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tm_command].name = "num_of_tm_command";
    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tm_command].doc = "";
    
    submodule_data->defines[dnx_data_techsupport_tm_define_num_of_tm_command].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_techsupport_tm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data techsupport tm tables");

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].name = "tm_block";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].doc = "";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].size_of_values = sizeof(dnx_data_techsupport_tm_tm_block_t);
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].entry_get = dnx_data_techsupport_tm_tm_block_entry_str_get;

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].nof_keys = 1;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].keys[0].name = "block";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values, dnxc_data_value_t, submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].nof_values, "_dnx_data_techsupport_tm_table_tm_block table values");
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[0].name = "block_name";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[0].type = "char *";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[0].doc = "block name";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[0].offset = UTILEX_OFFSETOF(dnx_data_techsupport_tm_tm_block_t, block_name);
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[1].name = "is_mask_ecc";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[1].type = "uint32";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[1].doc = " EID#8021 before performing cpu access read, is it need to disable ecc protection";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[1].offset = UTILEX_OFFSETOF(dnx_data_techsupport_tm_tm_block_t, is_mask_ecc);
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[2].name = "ecc_mask_words";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[2].type = "uint32";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[2].doc = "number of words proctected by ecc ";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_block].values[2].offset = UTILEX_OFFSETOF(dnx_data_techsupport_tm_tm_block_t, ecc_mask_words);

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].name = "tm_command";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].doc = "";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].size_of_values = sizeof(dnx_data_techsupport_tm_tm_command_t);
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].entry_get = dnx_data_techsupport_tm_tm_command_entry_str_get;

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].nof_keys = 1;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].keys[0].name = "index";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].values, dnxc_data_value_t, submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].nof_values, "_dnx_data_techsupport_tm_table_tm_command table values");
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].values[0].name = "command";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].values[0].type = "char *";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].values[0].doc = "excute command";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_command].values[0].offset = UTILEX_OFFSETOF(dnx_data_techsupport_tm_tm_command_t, command);

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].name = "tm_mem";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].doc = "";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].size_of_values = sizeof(dnx_data_techsupport_tm_tm_mem_t);
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].entry_get = dnx_data_techsupport_tm_tm_mem_entry_str_get;

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].nof_keys = 2;
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].keys[0].name = "block";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].keys[0].doc = "";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].keys[1].name = "index";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].values, dnxc_data_value_t, submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].nof_values, "_dnx_data_techsupport_tm_table_tm_mem table values");
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].values[0].name = "name";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].values[0].type = "char *";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].values[0].doc = "table name";
    submodule_data->tables[dnx_data_techsupport_tm_table_tm_mem].values[0].offset = UTILEX_OFFSETOF(dnx_data_techsupport_tm_tm_mem_t, name);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_techsupport_tm_feature_get(
    int unit,
    dnx_data_techsupport_tm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, feature);
}


uint32
dnx_data_techsupport_tm_num_of_tables_per_block_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_define_num_of_tables_per_block);
}

uint32
dnx_data_techsupport_tm_num_of_tm_command_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_define_num_of_tm_command);
}



const dnx_data_techsupport_tm_tm_block_t *
dnx_data_techsupport_tm_tm_block_get(
    int unit,
    int block)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_block);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, 0);
    return (const dnx_data_techsupport_tm_tm_block_t *) data;

}

const dnx_data_techsupport_tm_tm_command_t *
dnx_data_techsupport_tm_tm_command_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_command);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_techsupport_tm_tm_command_t *) data;

}

const dnx_data_techsupport_tm_tm_mem_t *
dnx_data_techsupport_tm_tm_mem_get(
    int unit,
    int block,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_mem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block, index);
    return (const dnx_data_techsupport_tm_tm_mem_t *) data;

}


shr_error_e
dnx_data_techsupport_tm_tm_block_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_techsupport_tm_tm_block_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_block);
    data = (const dnx_data_techsupport_tm_tm_block_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->block_name == NULL ? "" : data->block_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_mask_ecc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ecc_mask_words);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_techsupport_tm_tm_command_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_techsupport_tm_tm_command_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_command);
    data = (const dnx_data_techsupport_tm_tm_command_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->command == NULL ? "" : data->command);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_techsupport_tm_tm_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_techsupport_tm_tm_mem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_mem);
    data = (const dnx_data_techsupport_tm_tm_mem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->name == NULL ? "" : data->name);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_techsupport_tm_tm_block_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_block);

}

const dnxc_data_table_info_t *
dnx_data_techsupport_tm_tm_command_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_command);

}

const dnxc_data_table_info_t *
dnx_data_techsupport_tm_tm_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_techsupport, dnx_data_techsupport_submodule_tm, dnx_data_techsupport_tm_table_tm_mem);

}



shr_error_e
dnx_data_techsupport_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "techsupport";
    module_data->nof_submodules = _dnx_data_techsupport_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data techsupport submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_techsupport_tm_init(unit, &module_data->submodules[dnx_data_techsupport_submodule_tm]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_techsupport_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_techsupport_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

