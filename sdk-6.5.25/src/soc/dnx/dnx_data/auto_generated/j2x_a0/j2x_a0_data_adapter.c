
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
j2x_a0_dnx_data_adapter_rx_constant_header_size_set(
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
j2x_a0_dnx_data_adapter_general_lib_ver_set(
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
j2x_a0_dnx_data_adapter_general_oamp_ms_id_set(
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
j2x_a0_dnx_data_adapter_general_Injection_set(
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

    
    table->values[0].default_val = "PRSR_J2X_A0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_general_Injection_t, (1 + 1  ), "data of dnx_data_adapter_general_table_Injection");

    
    default_data = (dnx_data_adapter_general_Injection_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->Injected_packet_name = "PRSR_J2X_A0";
    
    data = (dnx_data_adapter_general_Injection_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
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
j2x_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
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
j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_key_map(
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_val)
    {
        case 170:
            *key0_index = 0;
            break;
        case 195:
            *key0_index = 1;
            break;
        case 250:
            *key0_index = 2;
            break;
        case 174:
            *key0_index = 3;
            break;
        case 152:
            *key0_index = 4;
            break;
        case 251:
            *key0_index = 5;
            break;
        case 670:
            *key0_index = 6;
            break;
        case 252:
            *key0_index = 7;
            break;
        case 110:
            *key0_index = 8;
            break;
        case 671:
            *key0_index = 9;
            break;
        case 672:
            *key0_index = 10;
            break;
        case 134:
            *key0_index = 11;
            break;
        case 113:
            *key0_index = 12;
            break;
        case 135:
            *key0_index = 13;
            break;
        case 278:
            *key0_index = 14;
            break;
        case 138:
            *key0_index = 15;
            break;
        case 117:
            *key0_index = 16;
            break;
        case 72:
            *key0_index = 17;
            break;
        case 10:
            *key0_index = 18;
            break;
        case 11:
            *key0_index = 19;
            break;
        case 12:
            *key0_index = 20;
            break;
        case 240:
            *key0_index = 21;
            break;
        case 142:
            *key0_index = 22;
            break;
        case 241:
            *key0_index = 23;
            break;
        case 243:
            *key0_index = 24;
            break;
        case 1:
            *key0_index = 25;
            break;
        case 2:
            *key0_index = 26;
            break;
        case 244:
            *key0_index = 27;
            break;
        case 3:
            *key0_index = 28;
            break;
        case 245:
            *key0_index = 29;
            break;
        case 125:
            *key0_index = 30;
            break;
        case 246:
            *key0_index = 31;
            break;
        case 4:
            *key0_index = 32;
            break;
        case 148:
            *key0_index = 33;
            break;
        case 5:
            *key0_index = 34;
            break;
        case 247:
            *key0_index = 35;
            break;
        case 6:
            *key0_index = 36;
            break;
        case 248:
            *key0_index = 37;
            break;
        case 249:
            *key0_index = 38;
            break;
        case 7:
            *key0_index = 39;
            break;
        case 8:
            *key0_index = 40;
            break;
        case 669:
            *key0_index = 41;
            break;
        case 9:
            *key0_index = 42;
            break;
        case 82:
            *key0_index = 43;
            break;
        default:
            *key0_index = -1;
            break;
    }
    switch (key1_val)
    {
        case DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_LEXEM:
            *key1_index = 0;
            break;
        case DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 1;
            break;
        case DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_ESEM:
            *key1_index = 2;
            break;
        case DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 3;
            break;
        case DNX_PP_STAGE_VTT5+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 4;
            break;
        case DNX_PP_STAGE_LLR+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 5;
            break;
        case DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_SEXEM_3:
            *key1_index = 6;
            break;
        case DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 7;
            break;
        case DNX_PP_STAGE_VTT4+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 8;
            break;
        case DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 9;
            break;
        case DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 10;
            break;
        case DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_GLEM_1:
            *key1_index = 11;
            break;
        case DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 12;
            break;
        case DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_SEXEM_3:
            *key1_index = 13;
            break;
        case DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_LEXEM:
            *key1_index = 14;
            break;
        case DNX_PP_STAGE_IFWD1+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 15;
            break;
        case DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1:
            *key1_index = 16;
            break;
        case DNX_PP_STAGE_VTT3+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 17;
            break;
        case DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1:
            *key1_index = 18;
            break;
        case DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_SEXEM_3:
            *key1_index = 19;
            break;
        case DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_SEXEM_2:
            *key1_index = 20;
            break;
        case DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_PPMC:
            *key1_index = 21;
            break;
        case DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_LEXEM:
            *key1_index = 22;
            break;
        case DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 23;
            break;
        case DNX_PP_STAGE_VTT2+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 24;
            break;
        case DNX_PP_STAGE_ERPARSER+100*DBAL_PHYSICAL_TABLE_PPMC:
            *key1_index = 25;
            break;
        case DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_TCAM:
            *key1_index = 26;
            break;
        case DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_SEXEM_1:
            *key1_index = 27;
            break;
        default:
            *key1_index = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_key_reverse_map(
    int unit,
    int key0_index,
    int key1_index,
    int *key0_val,
    int *key1_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_index)
    {
        case 0:
            *key0_val = 170;
            break;
        case 1:
            *key0_val = 195;
            break;
        case 2:
            *key0_val = 250;
            break;
        case 3:
            *key0_val = 174;
            break;
        case 4:
            *key0_val = 152;
            break;
        case 5:
            *key0_val = 251;
            break;
        case 6:
            *key0_val = 670;
            break;
        case 7:
            *key0_val = 252;
            break;
        case 8:
            *key0_val = 110;
            break;
        case 9:
            *key0_val = 671;
            break;
        case 10:
            *key0_val = 672;
            break;
        case 11:
            *key0_val = 134;
            break;
        case 12:
            *key0_val = 113;
            break;
        case 13:
            *key0_val = 135;
            break;
        case 14:
            *key0_val = 278;
            break;
        case 15:
            *key0_val = 138;
            break;
        case 16:
            *key0_val = 117;
            break;
        case 17:
            *key0_val = 72;
            break;
        case 18:
            *key0_val = 10;
            break;
        case 19:
            *key0_val = 11;
            break;
        case 20:
            *key0_val = 12;
            break;
        case 21:
            *key0_val = 240;
            break;
        case 22:
            *key0_val = 142;
            break;
        case 23:
            *key0_val = 241;
            break;
        case 24:
            *key0_val = 243;
            break;
        case 25:
            *key0_val = 1;
            break;
        case 26:
            *key0_val = 2;
            break;
        case 27:
            *key0_val = 244;
            break;
        case 28:
            *key0_val = 3;
            break;
        case 29:
            *key0_val = 245;
            break;
        case 30:
            *key0_val = 125;
            break;
        case 31:
            *key0_val = 246;
            break;
        case 32:
            *key0_val = 4;
            break;
        case 33:
            *key0_val = 148;
            break;
        case 34:
            *key0_val = 5;
            break;
        case 35:
            *key0_val = 247;
            break;
        case 36:
            *key0_val = 6;
            break;
        case 37:
            *key0_val = 248;
            break;
        case 38:
            *key0_val = 249;
            break;
        case 39:
            *key0_val = 7;
            break;
        case 40:
            *key0_val = 8;
            break;
        case 41:
            *key0_val = 669;
            break;
        case 42:
            *key0_val = 9;
            break;
        case 43:
            *key0_val = 82;
            break;
        default:
            *key0_val = -1;
            break;
    }
    switch (key1_index)
    {
        case 0:
            *key1_val = DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_LEXEM;
            break;
        case 1:
            *key1_val = DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 2:
            *key1_val = DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_ESEM;
            break;
        case 3:
            *key1_val = DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 4:
            *key1_val = DNX_PP_STAGE_VTT5+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 5:
            *key1_val = DNX_PP_STAGE_LLR+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 6:
            *key1_val = DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_SEXEM_3;
            break;
        case 7:
            *key1_val = DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 8:
            *key1_val = DNX_PP_STAGE_VTT4+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 9:
            *key1_val = DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 10:
            *key1_val = DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 11:
            *key1_val = DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_GLEM_1;
            break;
        case 12:
            *key1_val = DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 13:
            *key1_val = DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_SEXEM_3;
            break;
        case 14:
            *key1_val = DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_LEXEM;
            break;
        case 15:
            *key1_val = DNX_PP_STAGE_IFWD1+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 16:
            *key1_val = DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1;
            break;
        case 17:
            *key1_val = DNX_PP_STAGE_VTT3+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 18:
            *key1_val = DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1;
            break;
        case 19:
            *key1_val = DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_SEXEM_3;
            break;
        case 20:
            *key1_val = DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_SEXEM_2;
            break;
        case 21:
            *key1_val = DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_PPMC;
            break;
        case 22:
            *key1_val = DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_LEXEM;
            break;
        case 23:
            *key1_val = DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 24:
            *key1_val = DNX_PP_STAGE_VTT2+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 25:
            *key1_val = DNX_PP_STAGE_ERPARSER+100*DBAL_PHYSICAL_TABLE_PPMC;
            break;
        case 26:
            *key1_val = DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_TCAM;
            break;
        case 27:
            *key1_val = DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_SEXEM_1;
            break;
        default:
            *key1_val = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_set(
    int unit)
{
    int caller_id_index;
    int stage_mdb_key_index;
    dnx_data_adapter_mdb_lookup_caller_id_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_mdb;
    int table_index = dnx_data_adapter_mdb_table_lookup_caller_id_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 44;
    table->info_get.key_size[0] = 44;
    table->keys[1].size = 28;
    table->info_get.key_size[1] = 28;

    
    table->values[0].default_val = "DNX_PP_STAGE_NOF";
    table->values[1].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    table->values[2].default_val = "NotFound";
    
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
    default_data->stage = DNX_PP_STAGE_NOF;
    default_data->mdb = DBAL_NOF_PHYSICAL_TABLES;
    default_data->interface = "NotFound";
    
    for (caller_id_index = 0; caller_id_index < table->keys[0].size; caller_id_index++)
    {
        for (stage_mdb_key_index = 0; stage_mdb_key_index < table->keys[1].size; stage_mdb_key_index++)
        {
            data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, caller_id_index, stage_mdb_key_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    table->key_map = j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_key_map;
    table->key_map_reverse = j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_key_reverse_map;
    
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 249, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_EOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 170, DNX_PP_STAGE_IFWD1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IFWD1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 174, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 195, DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF3;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 243, DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_EPMF;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_ERPRP;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, DNX_PP_STAGE_LLR+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_LLR;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, DNX_PP_STAGE_LLR+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_LLR;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_SEXEM_1);
    data->stage = DNX_PP_STAGE_VTT1;
    data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, DNX_PP_STAGE_VTT1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_VTT1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, DNX_PP_STAGE_VTT2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_VTT2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, DNX_PP_STAGE_VTT3+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_VTT3;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, DNX_PP_STAGE_VTT4+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_VTT4;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, DNX_PP_STAGE_VTT5+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_VTT5;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 174, DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IFWD2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 669, DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1);
    data->stage = DNX_PP_STAGE_IFWD2;
    data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 670, DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1);
    data->stage = DNX_PP_STAGE_IFWD2;
    data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 671, DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1);
    data->stage = DNX_PP_STAGE_IFWD2;
    data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 672, DNX_PP_STAGE_IFWD2+100*DBAL_PHYSICAL_TABLE_IOEM_1);
    data->stage = DNX_PP_STAGE_IFWD2;
    data->mdb = DBAL_PHYSICAL_TABLE_IOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 241, DNX_PP_STAGE_ERPARSER+100*DBAL_PHYSICAL_TABLE_PPMC);
    data->stage = DNX_PP_STAGE_ERPARSER;
    data->mdb = DBAL_PHYSICAL_TABLE_PPMC;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 240, DNX_PP_STAGE_ERPRP+100*DBAL_PHYSICAL_TABLE_GLEM_1);
    data->stage = DNX_PP_STAGE_ERPRP;
    data->mdb = DBAL_PHYSICAL_TABLE_GLEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 135, DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_EPMF;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 241, DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_PPMC);
    data->stage = DNX_PP_STAGE_ETPARSER;
    data->mdb = DBAL_PHYSICAL_TABLE_PPMC;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, DNX_PP_STAGE_ETPARSER+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_ETPARSER;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 250, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_EOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 251, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_EOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 252, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_EOEM_1);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_EOEM_1;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 246, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_ESEM);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_ESEM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 278, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF3;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 278, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 244, DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_EPMF;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF3;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 247, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_ESEM);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_ESEM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_LEXEM);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_LEXEM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_LEXEM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_LEXEM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, DNX_PP_STAGE_IPMF1+100*DBAL_PHYSICAL_TABLE_SEXEM_3);
    data->stage = DNX_PP_STAGE_IPMF1;
    data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_3;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_SEXEM_3);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_3;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, DNX_PP_STAGE_IPMF3+100*DBAL_PHYSICAL_TABLE_SEXEM_3);
    data->stage = DNX_PP_STAGE_IPMF3;
    data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_3;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 245, DNX_PP_STAGE_EPMF+100*DBAL_PHYSICAL_TABLE_LEXEM);
    data->stage = DNX_PP_STAGE_EPMF;
    data->mdb = DBAL_PHYSICAL_TABLE_LEXEM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, DNX_PP_STAGE_IPMF2+100*DBAL_PHYSICAL_TABLE_TCAM);
    data->stage = DNX_PP_STAGE_IPMF2;
    data->mdb = DBAL_PHYSICAL_TABLE_TCAM;
    data->interface = "UnknownInterface";
    data = (dnx_data_adapter_mdb_lookup_caller_id_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, 248, DNX_PP_STAGE_TERM+100*DBAL_PHYSICAL_TABLE_SEXEM_2);
    data->stage = DNX_PP_STAGE_TERM;
    data->mdb = DBAL_PHYSICAL_TABLE_SEXEM_2;
    data->interface = "UnknownInterface";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2x_a0_data_adapter_attach(
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
    
    submodule_index = dnx_data_adapter_submodule_rx;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_rx_define_constant_header_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_adapter_rx_constant_header_size_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_general_define_lib_ver;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_adapter_general_lib_ver_set;
    data_index = dnx_data_adapter_general_define_oamp_ms_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_adapter_general_oamp_ms_id_set;

    

    
    data_index = dnx_data_adapter_general_table_Injection;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_adapter_general_Injection_set;
    
    submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set;
    data_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_mdb;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_adapter_mdb_table_lookup_caller_id_mapping;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_adapter_mdb_lookup_caller_id_mapping_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

