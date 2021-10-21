
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>








static shr_error_e
jr2_a0_dnx_data_adapter_tx_loopback_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_tx;
    int define_index = dnx_data_adapter_tx_define_loopback_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "adapter_loopback_enable";
    define->property.doc = 
        "\n"
        "Use loopback in the adapter server when receiving packets.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_adapter_rx_constant_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_rx;
    int define_index = dnx_data_adapter_rx_define_constant_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_adapter_general_nof_sub_units_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int define_index = dnx_data_adapter_general_define_nof_sub_units;
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
jr2_a0_dnx_data_adapter_general_lib_ver_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int define_index = dnx_data_adapter_general_define_lib_ver;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_general_oamp_ms_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int define_index = dnx_data_adapter_general_define_oamp_ms_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 108;

    
    define->data = 108;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_adapter_general_Injection_set(
    int unit)
{
    dnx_data_adapter_general_Injection_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int table_index = dnx_data_adapter_general_table_Injection;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "PRSR_JR2_A0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_general_Injection_t, (1 + 1  ), "data of dnx_data_adapter_general_table_Injection");

    
    default_data = (dnx_data_adapter_general_Injection_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->Injected_packet_name = "PRSR_JR2_A0";
    
    data = (dnx_data_adapter_general_Injection_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_do_collect_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_do_collect_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = "custom_feature";
    define->property.doc = 
        "\n"
        "Use do_collect in adapter to collect writes into one bulk write.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "adapter_do_collect_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index;
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
jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_mems_set(
    int unit)
{
    int index_index;
    dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int table_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_mems;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_reg_mem_access_clear_on_read_mems_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_adapter_reg_mem_access_table_clear_on_read_mems");

    
    default_data = (dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_regs_set(
    int unit)
{
    int index_index;
    dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int table_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_reg_mem_access_clear_on_read_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_adapter_reg_mem_access_table_clear_on_read_regs");

    
    default_data = (dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_set(
    int unit)
{
    int caller_id_index;
    int stage_index;
    dnx_data_adapter_mdb_lookup_caller_id_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_mdb;
    int table_index = dnx_data_adapter_mdb_table_lookup_caller_id_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1000;
    table->info_get.key_size[0] = 1000;
    table->keys[1].size = DNX_PP_STAGE_NOF;
    table->info_get.key_size[1] = DNX_PP_STAGE_NOF;

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    table->values[1].default_val = "NotFound";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_mdb_lookup_caller_id_mapping_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_adapter_mdb_table_lookup_caller_id_mapping");

    
    default_data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdb = DBAL_NOF_PHYSICAL_TABLES;
    default_data->interface = "NotFound";
    
    for (caller_id_index = 0; caller_id_index < table->keys[0].size; caller_id_index++)
    {
        for (stage_index = 0; stage_index < table->keys[1].size; stage_index++)
        {
            data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, caller_id_index, stage_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (240 < table->keys[0].size && DNX_PP_STAGE_ERPRP < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 240, DNX_PP_STAGE_ERPRP);
        data->mdb = DBAL_PHYSICAL_TABLE_GLEM_1;
        data->interface = "Empty";
    }
    if (241 < table->keys[0].size && DNX_PP_STAGE_ERPARSER < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 241, DNX_PP_STAGE_ERPARSER);
        data->mdb = DBAL_PHYSICAL_TABLE_PPMC;
        data->interface = "Empty";
    }
    if (72 < table->keys[0].size && DNX_PP_STAGE_ETPARSER < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, DNX_PP_STAGE_ETPARSER);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (241 < table->keys[0].size && DNX_PP_STAGE_ETPARSER < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 241, DNX_PP_STAGE_ETPARSER);
        data->mdb = DBAL_PHYSICAL_TABLE_PPMC;
        data->interface = "Empty";
    }
    if (246 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 246, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_ESEM;
        data->interface = "0";
    }
    if (247 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 247, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_ESEM;
        data->interface = "1";
    }
    if (248 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 248, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->interface = "Empty";
    }
    if (249 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 249, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "0";
    }
    if (250 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 250, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "1";
    }
    if (251 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 251, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "2";
    }
    if (252 < table->keys[0].size && DNX_PP_STAGE_TERM < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 252, DNX_PP_STAGE_TERM);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "3";
    }
    if (669 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 669, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "0";
    }
    if (670 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 670, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "1";
    }
    if (671 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 671, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "2";
    }
    if (672 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 672, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
        data->interface = "3";
    }
    if (105 < table->keys[0].size && DNX_PP_STAGE_LLR < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, DNX_PP_STAGE_LLR);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (110 < table->keys[0].size && DNX_PP_STAGE_LLR < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, DNX_PP_STAGE_LLR);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (125 < table->keys[0].size && DNX_PP_STAGE_LLR < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, DNX_PP_STAGE_LLR);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (6 < table->keys[0].size && DNX_PP_STAGE_VTT1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, DNX_PP_STAGE_VTT1);
        data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->interface = "6";
    }
    if (130 < table->keys[0].size && DNX_PP_STAGE_VTT1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, DNX_PP_STAGE_VTT1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (134 < table->keys[0].size && DNX_PP_STAGE_VTT1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, DNX_PP_STAGE_VTT1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (138 < table->keys[0].size && DNX_PP_STAGE_VTT2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, DNX_PP_STAGE_VTT2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (142 < table->keys[0].size && DNX_PP_STAGE_VTT2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, DNX_PP_STAGE_VTT2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (134 < table->keys[0].size && DNX_PP_STAGE_VTT3 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, DNX_PP_STAGE_VTT3);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (138 < table->keys[0].size && DNX_PP_STAGE_VTT3 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, DNX_PP_STAGE_VTT3);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (111 < table->keys[0].size && DNX_PP_STAGE_VTT4 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, DNX_PP_STAGE_VTT4);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (113 < table->keys[0].size && DNX_PP_STAGE_VTT4 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, DNX_PP_STAGE_VTT4);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (115 < table->keys[0].size && DNX_PP_STAGE_VTT5 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, DNX_PP_STAGE_VTT5);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (117 < table->keys[0].size && DNX_PP_STAGE_VTT5 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, DNX_PP_STAGE_VTT5);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (157 < table->keys[0].size && DNX_PP_STAGE_IFWD1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 157, DNX_PP_STAGE_IFWD1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (171 < table->keys[0].size && DNX_PP_STAGE_IFWD1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 171, DNX_PP_STAGE_IFWD1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (161 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 161, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (175 < table->keys[0].size && DNX_PP_STAGE_IFWD2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 175, DNX_PP_STAGE_IFWD2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "SkipSignal";
    }
    if (148 < table->keys[0].size && DNX_PP_STAGE_IPMF1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, DNX_PP_STAGE_IPMF1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "148";
    }
    if (228 < table->keys[0].size && DNX_PP_STAGE_IPMF1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 228, DNX_PP_STAGE_IPMF1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "228";
    }
    if (152 < table->keys[0].size && DNX_PP_STAGE_IPMF1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, DNX_PP_STAGE_IPMF1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "152";
    }
    if (182 < table->keys[0].size && DNX_PP_STAGE_IPMF1 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 182, DNX_PP_STAGE_IPMF1);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "182";
    }
    if (148 < table->keys[0].size && DNX_PP_STAGE_IPMF2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, DNX_PP_STAGE_IPMF2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "148";
    }
    if (228 < table->keys[0].size && DNX_PP_STAGE_IPMF2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 228, DNX_PP_STAGE_IPMF2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "228";
    }
    if (152 < table->keys[0].size && DNX_PP_STAGE_IPMF2 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, DNX_PP_STAGE_IPMF2);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "152";
    }
    if (182 < table->keys[0].size && DNX_PP_STAGE_IPMF3 < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 182, DNX_PP_STAGE_IPMF3);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "182";
    }
    if (135 < table->keys[0].size && DNX_PP_STAGE_EPMF < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 135, DNX_PP_STAGE_EPMF);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "135";
    }
    if (128 < table->keys[0].size && DNX_PP_STAGE_EPMF < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, DNX_PP_STAGE_EPMF);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "128";
    }
    if (80 < table->keys[0].size && DNX_PP_STAGE_ERPRP < table->keys[1].size)
    {
        data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, DNX_PP_STAGE_ERPRP);
        data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
        data->interface = "80";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_adapter_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_adapter;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_adapter_submodule_tx;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_tx_define_loopback_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_tx_loopback_enable_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_rx;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_rx_define_constant_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_rx_constant_header_size_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_general_define_nof_sub_units;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_general_nof_sub_units_set;
    data_index = dnx_data_adapter_general_define_lib_ver;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_general_lib_ver_set;
    data_index = dnx_data_adapter_general_define_oamp_ms_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_general_oamp_ms_id_set;

    

    
    data_index = dnx_data_adapter_general_table_Injection;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_general_Injection_set;
    
    submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_reg_mem_access_define_do_collect_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_do_collect_enable_set;
    data_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set;
    data_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set;
    data_index = dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_set;

    

    
    data_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_mems;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_mems_set;
    data_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_regs_set;
    
    submodule_index = dnx_data_adapter_submodule_mdb;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_adapter_mdb_table_lookup_caller_id_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

