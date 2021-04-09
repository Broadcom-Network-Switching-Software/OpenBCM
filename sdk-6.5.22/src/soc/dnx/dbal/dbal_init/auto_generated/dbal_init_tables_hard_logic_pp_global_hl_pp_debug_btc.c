
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_termination_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_TERMINATION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_TERMINATION , is_valid , "BTC_INSTRUCTION_TERMINATION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_TERMINATION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TERM_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_TERM_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 12);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 12;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_TERMINATION, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_TERM_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 12; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_TERM_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_termination_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_TERMINATION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_forwarding_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_FORWARDING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_FORWARDING , is_valid , "BTC_INSTRUCTION_FORWARDING" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_FORWARDING" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_FWD_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 12);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 12;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_FORWARDING, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_FWD_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 12; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_FWD_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_forwarding_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_FORWARDING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_1_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_ENCAP_1;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_1 , is_valid , "BTC_INSTRUCTION_ENCAP_1" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_ENCAP_1" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_1_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_ENCAP_1_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_1, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_ENC_1_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_ENC_1_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_encap_1_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_1, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_ENCAP_2;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_2 , is_valid , "BTC_INSTRUCTION_ENCAP_2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_ENCAP_2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_2_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_ENCAP_2_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_2, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_ENC_2_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_ENC_2_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_encap_2_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_3_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_ENCAP_3;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_3 , is_valid , "BTC_INSTRUCTION_ENCAP_3" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_ENCAP_3" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_3_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_ENCAP_3_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_3, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_ENC_3_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_ENC_3_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_encap_3_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_3, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_4_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_ENCAP_4;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_4 , is_valid , "BTC_INSTRUCTION_ENCAP_4" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_ENCAP_4" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_4_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_ENCAP_4_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_4, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_ENC_4_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_ENC_4_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_encap_4_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_4, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_5_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_ENCAP_5;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_5 , is_valid , "BTC_INSTRUCTION_ENCAP_5" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_ENCAP_5" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_5_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_ENCAP_5_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_5, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_ENC_5_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_ENC_5_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_encap_5_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_ENCAP_5, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_trap_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BTC_INSTRUCTION_TRAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BTC_INSTRUCTION_TRAP , is_valid , "BTC_INSTRUCTION_TRAP" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "BTC_INSTRUCTION_TRAP" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRAP_CONTEXT_ID , DBAL_FIELD_TYPE_DEF_TRAP_CONTEXT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_DOUBLE_INST , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DOUBLE_INSTRUCTION , DBAL_FIELD_TYPE_DEF_BTC_DOUBLE_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BTC_INSTRUCTION_TRAP, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NOF_DOUBLE_INST;
                access_params->inst_idx = instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_TRAP_BIT_CRUNCHER_PROGRAMm;
                    
                    fieldHwEntityId = NOF_INSTRUCTIONS_IN_PAIRSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                
                {
                    int field_instance = 0;
                    
                    for (field_instance = 0; field_instance < 8; field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        
                        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                        
                        
                        access_params->access_field_id = DBAL_FIELD_DOUBLE_INSTRUCTION;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                        
                        {
                            int regMemHwEntityId = INVALIDm;
                            int fieldHwEntityId = INVALIDf;
                            
                            regMemHwEntityId = ETPPB_TRAP_BIT_CRUNCHER_PROGRAMm;
                            
                            fieldHwEntityId = INVALIDf;
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                            access_params->data_offset.formula_cb = btc_instruction_trap_double_instruction_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BTC_INSTRUCTION_TRAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_termination_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_forwarding_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_3_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_4_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_encap_5_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_btc_btc_instruction_trap_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
