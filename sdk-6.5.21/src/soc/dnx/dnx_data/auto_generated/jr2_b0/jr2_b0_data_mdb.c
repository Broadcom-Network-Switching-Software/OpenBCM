

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MDBDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mdb.h>













static shr_error_e
jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int define_index = dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size;
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
jr2_b0_dnx_data_mdb_pdbs_mdb_adapter_mapping_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_pdbs_mdb_adapter_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_pdbs;
    int table_index = dnx_data_mdb_pdbs_table_mdb_adapter_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_pdbs_mdb_adapter_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_pdbs_table_mdb_adapter_mapping");

    
    default_data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->memory_id = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_NONE < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_NONE, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_TCAM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_TCAM, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KBP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KBP, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_1, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_KAPS_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_KAPS_2, 0);
        data->memory_id = 1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->memory_id = 0;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_1, 0);
        data->memory_id = 172;
    }
    if (DBAL_PHYSICAL_TABLE_IVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IVSI, 0);
        data->memory_id = 175;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->memory_id = 1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->memory_id = 2;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_2, 0);
        data->memory_id = 173;
    }
    if (DBAL_PHYSICAL_TABLE_INLIF_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_INLIF_3, 0);
        data->memory_id = 174;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->memory_id = 10;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->memory_id = 5;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->memory_id = 6;
    }
    if (DBAL_PHYSICAL_TABLE_MAP < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_MAP, 0);
        data->memory_id = 176;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_1, 0);
        data->memory_id = 177;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_2, 0);
        data->memory_id = 178;
    }
    if (DBAL_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_FEC_3, 0);
        data->memory_id = 179;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->memory_id = 11;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->memory_id = 3;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->memory_id = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_1, 0);
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_2, 0);
        data->memory_id = 180;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_3, 0);
        data->memory_id = 181;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_4, 0);
        data->memory_id = 181;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_5 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_5, 0);
        data->memory_id = 182;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_6 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_6, 0);
        data->memory_id = 182;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_7 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_7, 0);
        data->memory_id = 183;
    }
    if (DBAL_PHYSICAL_TABLE_EEDB_8 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EEDB_8, 0);
        data->memory_id = 183;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->memory_id = 7;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->memory_id = 8;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->memory_id = 9;
    }
    if (DBAL_PHYSICAL_TABLE_EVSI < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EVSI, 0);
        data->memory_id = 184;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->memory_id = 13;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->memory_id = 14;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->memory_id = 15;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->memory_id = 16;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_pdbs_mdb_adapter_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->memory_id = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_b0_dnx_data_mdb_em_entry_type_parser_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int feature_index = dnx_data_mdb_em_entry_type_parser;
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
jr2_b0_dnx_data_mdb_em_age_support_per_entry_size_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_support_per_entry_size_ratio;
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
jr2_b0_dnx_data_mdb_em_flush_tcam_rule_counter_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flush_tcam_rule_counter_support;
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
jr2_b0_dnx_data_mdb_em_defragmentation_priority_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_defragmentation_priority_supported;
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
jr2_b0_dnx_data_mdb_em_age_profile_per_ratio_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_age_profile_per_ratio_support;
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
jr2_b0_dnx_data_mdb_em_flex_mag_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flex_mag_supported;
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
jr2_b0_dnx_data_mdb_em_flex_fully_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_flex_fully_supported;
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
jr2_b0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int define_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support;
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
jr2_b0_dnx_data_mdb_em_em_info_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "INVALIDm";
    table->values[2].default_val = "0";
    table->values[3].default_val = "INVALIDr";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_info");

    
    default_data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tid_size = 0;
    default_data->em_interface = INVALIDm;
    default_data->entry_size = 0;
    default_data->status_reg = INVALIDr;
    default_data->step_table_size = 0;
    default_data->ovf_cam_size = 0;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->tid_size = 2;
        data->em_interface = MDB_MC_IDm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_305r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->tid_size = 3;
        data->em_interface = MDB_RMEPm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_594r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_119r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_2m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_143r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ISEM_3m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_167r;
        data->step_table_size = 1024;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_327r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->tid_size = 2;
        data->em_interface = MDB_GLEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_345r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_LEMm;
        data->entry_size = 257;
        data->status_reg = MDB_REG_221r;
        data->step_table_size = 2048;
        data->ovf_cam_size = 8;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_ESEMm;
        data->entry_size = 137;
        data->status_reg = MDB_REG_418r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_441r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_2m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_464r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_3m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_515r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EXEM_4m;
        data->entry_size = 257;
        data->status_reg = MDB_REG_568r;
        data->step_table_size = 512;
        data->ovf_cam_size = 2;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_372r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_EOEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_391r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_0m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_260r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->tid_size = 6;
        data->em_interface = MDB_IOEM_1m;
        data->entry_size = 137;
        data->status_reg = MDB_REG_279r;
        data->step_table_size = 512;
        data->ovf_cam_size = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_step_table_pdb_max_depth_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_step_table_pdb_max_depth_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_step_table_pdb_max_depth;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "8";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_step_table_pdb_max_depth_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_step_table_pdb_max_depth");

    
    default_data = (dnx_data_mdb_em_step_table_pdb_max_depth_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_depth = 8;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_step_table_pdb_max_depth_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_16_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_16_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_16_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_16_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_16_info");

    
    default_data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_1;
        data->mdb_item_0_field_size = 17;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_2;
        data->mdb_item_0_field_size = 15;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ISEM_3;
        data->mdb_item_0_field_size = 15;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_LEM;
        data->mdb_item_0_field_size = 12;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_0;
        data->mdb_item_0_field_size = 9;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_IOEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_MC_ID;
        data->mdb_item_0_field_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_0;
        data->mdb_item_0_field_size = 13;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_GLEM_1;
        data->mdb_item_0_field_size = 13;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_0;
        data->mdb_item_0_field_size = 9;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EOEM_1;
        data->mdb_item_0_field_size = 9;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_ESEM;
        data->mdb_item_0_field_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_1;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_2;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_3;
        data->mdb_item_0_field_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_EXEM_4;
        data->mdb_item_0_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_16_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_16_RMEP;
        data->mdb_item_0_field_size = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_21_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_21_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_21_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_21_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_21_info");

    
    default_data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_LEM;
        data->mdb_item_0_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_EXEM_3;
        data->mdb_item_0_field_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_21_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_21_EXEM_4;
        data->mdb_item_0_field_size = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_23_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_23_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_23_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_23_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_23_info");

    
    default_data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_field_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_LEM;
        data->mdb_item_0_field_size = 12;
        data->mdb_item_1_field_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_EXEM_3;
        data->mdb_item_0_field_size = 5;
        data->mdb_item_1_field_size = 3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_23_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_23_EXEM_4;
        data->mdb_item_0_field_size = 3;
        data->mdb_item_1_field_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_24_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_24_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_24_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_24_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_24_info");

    
    default_data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 0;
    default_data->mdb_item_1_array_size = 0;
    default_data->mdb_item_1_field_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_LEM;
        data->mdb_item_0_array_size = 12;
        data->mdb_item_1_array_size = 12;
        data->mdb_item_1_field_size = 18;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_EXEM_3;
        data->mdb_item_0_array_size = 5;
        data->mdb_item_1_array_size = 5;
        data->mdb_item_1_field_size = 17;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_24_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_24_EXEM_4;
        data->mdb_item_0_array_size = 3;
        data->mdb_item_1_array_size = 3;
        data->mdb_item_1_field_size = 16;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_29_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_29_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_29_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_29_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_29_info");

    
    default_data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    default_data->mdb_item_0_array_size = 0;
    default_data->mdb_item_1_array_size = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_1;
        data->mdb_item_0_array_size = 8;
        data->mdb_item_1_array_size = 8;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_2;
        data->mdb_item_0_array_size = 7;
        data->mdb_item_1_array_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ISEM_3;
        data->mdb_item_0_array_size = 7;
        data->mdb_item_1_array_size = 7;
    }
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_LEM;
        data->mdb_item_0_array_size = 11;
        data->mdb_item_1_array_size = 11;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_0;
        data->mdb_item_0_array_size = 4;
        data->mdb_item_1_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_IOEM_1;
        data->mdb_item_0_array_size = 4;
        data->mdb_item_1_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_MC_ID < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_MC_ID, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_MC_ID;
        data->mdb_item_0_array_size = 5;
        data->mdb_item_1_array_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_0;
        data->mdb_item_0_array_size = 6;
        data->mdb_item_1_array_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_GLEM_1;
        data->mdb_item_0_array_size = 6;
        data->mdb_item_1_array_size = 6;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_0 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_0, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_0;
        data->mdb_item_0_array_size = 4;
        data->mdb_item_1_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EOEM_1;
        data->mdb_item_0_array_size = 4;
        data->mdb_item_1_array_size = 4;
    }
    if (MDB_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_ESEM;
        data->mdb_item_0_array_size = 5;
        data->mdb_item_1_array_size = 5;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_1;
        data->mdb_item_0_array_size = 2;
        data->mdb_item_1_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_2;
        data->mdb_item_0_array_size = 2;
        data->mdb_item_1_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_3;
        data->mdb_item_0_array_size = 2;
        data->mdb_item_1_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_EXEM_4;
        data->mdb_item_0_array_size = 2;
        data->mdb_item_1_array_size = 2;
    }
    if (MDB_PHYSICAL_TABLE_RMEP < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_29_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_RMEP, 0);
        data->dbal_table = DBAL_TABLE_MDB_29_RMEP;
        data->mdb_item_0_array_size = 2;
        data->mdb_item_1_array_size = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_emp_tables_mapping_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_emp_tables_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_emp_tables_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_emp_tables_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_emp_tables_mapping");

    
    default_data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->emp_age_cfg_table = 0;
    default_data->mdb_22_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_LEM, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_20_LEM;
        data->mdb_22_table = DBAL_TABLE_MDB_22_LEM;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_3, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_20_EXEM_3;
        data->mdb_22_table = DBAL_TABLE_MDB_22_EXEM_3;
    }
    if (MDB_PHYSICAL_TABLE_EXEM_4 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_emp_tables_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_EXEM_4, 0);
        data->emp_age_cfg_table = DBAL_TABLE_MDB_20_EXEM_4;
        data->mdb_22_table = DBAL_TABLE_MDB_22_EXEM_4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_34_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_34_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_34_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_34_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_34_info");

    
    default_data = (dnx_data_mdb_em_mdb_34_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_34_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_34_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_34_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_34_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_34_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_34_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_34_EXEM_4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_em_emp_set(
    int unit)
{
    int dbal_id_index;
    dnx_data_mdb_em_em_emp_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_em_emp;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "INVALIDm";
    table->values[1].default_val = "INVALIDm";
    table->values[2].default_val = "INVALIDf";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_em_emp_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_em_emp");

    
    default_data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->age_mem = INVALIDm;
    default_data->age_ovf_cam_mem = INVALIDm;
    default_data->emp_scan_status_field = INVALIDf;
    
    for (dbal_id_index = 0; dbal_id_index < table->keys[0].size; dbal_id_index++)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->age_mem = MDB_MEM_0200000m;
        data->age_ovf_cam_mem = MDB_MEM_22000m;
        data->emp_scan_status_field = ITEM_36_37f;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->age_mem = MDB_MEM_210000m;
        data->age_ovf_cam_mem = MDB_MEM_92000m;
        data->emp_scan_status_field = ITEM_36_37f;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_em_emp_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->age_mem = MDB_MEM_220000m;
        data->age_ovf_cam_mem = MDB_MEM_102000m;
        data->emp_scan_status_field = ITEM_36_37f;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_mdb_em_mdb_step_table_map_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_em_mdb_step_table_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_em;
    int table_index = dnx_data_mdb_em_table_mdb_step_table_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = DBAL_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_em_mdb_step_table_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_em_table_mdb_step_table_map");

    
    default_data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_PHYSICAL_TABLE_ISEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_ISEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ISEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ISEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_LEM;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_IOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_IOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_IOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_IOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_PPMC < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_PPMC, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_MC_ID;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_GLEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_GLEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_GLEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_GLEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EOEM_0;
    }
    if (DBAL_PHYSICAL_TABLE_EOEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_EOEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EOEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_ESEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_ESEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_ESEM;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_1;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_2;
    }
    if (DBAL_PHYSICAL_TABLE_SEXEM_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_SEXEM_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_3;
    }
    if (DBAL_PHYSICAL_TABLE_LEXEM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_LEXEM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_EXEM_4;
    }
    if (DBAL_PHYSICAL_TABLE_RMEP_EM < table->keys[0].size)
    {
        data = (dnx_data_mdb_em_mdb_step_table_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_PHYSICAL_TABLE_RMEP_EM, 0);
        data->dbal_table = DBAL_TABLE_MDB_41_RMEP;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_b0_dnx_data_mdb_direct_physical_address_ovf_enable_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int define_index = dnx_data_mdb_direct_define_physical_address_ovf_enable_val;
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
jr2_b0_dnx_data_mdb_direct_mdb_42_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_mdb_direct_mdb_42_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_direct;
    int table_index = dnx_data_mdb_direct_table_mdb_42_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_mdb_direct_mdb_42_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_mdb_direct_table_mdb_42_info");

    
    default_data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_MDB_42_FEC_1;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_MDB_42_FEC_2;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_mdb_direct_mdb_42_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_MDB_42_FEC_3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}










static shr_error_e
jr2_b0_dnx_data_mdb_feature_step_table_supports_mix_ar_algo_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_step_table_supports_mix_ar_algo;
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
jr2_b0_dnx_data_mdb_feature_eedb_bank_traffic_lock_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_eedb_bank_traffic_lock;
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
jr2_b0_dnx_data_mdb_feature_em_dfg_ovf_cam_disabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_dfg_ovf_cam_disabled;
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
jr2_b0_dnx_data_mdb_feature_em_mact_transplant_no_reply_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_transplant_no_reply;
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
jr2_b0_dnx_data_mdb_feature_em_mact_use_refresh_on_insert_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_mact_use_refresh_on_insert;
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
jr2_b0_dnx_data_mdb_feature_em_sbus_interface_shutdown_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mdb;
    int submodule_index = dnx_data_mdb_submodule_feature;
    int feature_index = dnx_data_mdb_feature_em_sbus_interface_shutdown;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_b0_data_mdb_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_mdb;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_mdb_submodule_dh;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_mdb_submodule_pdbs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_1_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_1_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_2_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_2_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_9_mdb_item_3_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_9_mdb_item_3_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_40_mdb_item_0_array_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_40_mdb_item_0_array_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_dynamic_memory_access_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_mact_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_mact_dynamic_memory_access_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_sbc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_kaps_dynamic_memory_access_dpc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_sbc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddha_dynamic_memory_access_dpc_memory_access_field_size_set;
    data_index = dnx_data_mdb_pdbs_define_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_pdbs_table_mdb_ddhb_dynamic_memory_access_memory_access_field_size_set;

    

    
    data_index = dnx_data_mdb_pdbs_table_mdb_adapter_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_pdbs_mdb_adapter_mapping_set;
    
    submodule_index = dnx_data_mdb_submodule_em;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_em_define_age_support_per_entry_size_ratio;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_age_support_per_entry_size_ratio_set;
    data_index = dnx_data_mdb_em_define_flush_tcam_rule_counter_support;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_flush_tcam_rule_counter_support_set;
    data_index = dnx_data_mdb_em_define_defragmentation_priority_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_defragmentation_priority_supported_set;
    data_index = dnx_data_mdb_em_define_age_profile_per_ratio_support;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_age_profile_per_ratio_support_set;
    data_index = dnx_data_mdb_em_define_flex_mag_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_flex_mag_supported_set;
    data_index = dnx_data_mdb_em_define_flex_fully_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_flex_fully_supported_set;
    data_index = dnx_data_mdb_em_define_scan_bank_participate_in_cuckoo_support;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_em_scan_bank_participate_in_cuckoo_support_set;

    
    data_index = dnx_data_mdb_em_entry_type_parser;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_em_entry_type_parser_set;

    
    data_index = dnx_data_mdb_em_table_em_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_em_info_set;
    data_index = dnx_data_mdb_em_table_step_table_pdb_max_depth;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_step_table_pdb_max_depth_set;
    data_index = dnx_data_mdb_em_table_mdb_16_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_16_info_set;
    data_index = dnx_data_mdb_em_table_mdb_21_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_21_info_set;
    data_index = dnx_data_mdb_em_table_mdb_23_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_23_info_set;
    data_index = dnx_data_mdb_em_table_mdb_24_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_24_info_set;
    data_index = dnx_data_mdb_em_table_mdb_29_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_29_info_set;
    data_index = dnx_data_mdb_em_table_mdb_emp_tables_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_emp_tables_mapping_set;
    data_index = dnx_data_mdb_em_table_mdb_34_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_34_info_set;
    data_index = dnx_data_mdb_em_table_em_emp;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_em_emp_set;
    data_index = dnx_data_mdb_em_table_mdb_step_table_map;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_em_mdb_step_table_map_set;
    
    submodule_index = dnx_data_mdb_submodule_direct;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mdb_direct_define_physical_address_ovf_enable_val;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_mdb_direct_physical_address_ovf_enable_val_set;

    

    
    data_index = dnx_data_mdb_direct_table_mdb_42_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_mdb_direct_mdb_42_info_set;
    
    submodule_index = dnx_data_mdb_submodule_eedb;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_mdb_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_mdb_feature_step_table_supports_mix_ar_algo;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_step_table_supports_mix_ar_algo_set;
    data_index = dnx_data_mdb_feature_eedb_bank_traffic_lock;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_eedb_bank_traffic_lock_set;
    data_index = dnx_data_mdb_feature_em_dfg_ovf_cam_disabled;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_em_dfg_ovf_cam_disabled_set;
    data_index = dnx_data_mdb_feature_em_mact_transplant_no_reply;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_em_mact_transplant_no_reply_set;
    data_index = dnx_data_mdb_feature_em_mact_use_refresh_on_insert;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_em_mact_use_refresh_on_insert_set;
    data_index = dnx_data_mdb_feature_em_sbus_interface_shutdown;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_mdb_feature_em_sbus_interface_shutdown_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

