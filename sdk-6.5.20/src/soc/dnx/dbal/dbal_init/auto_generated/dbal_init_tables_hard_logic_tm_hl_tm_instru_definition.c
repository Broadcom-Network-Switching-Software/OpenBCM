
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL , is_valid , "INST_SYNCED_COUNTERS_CTRL" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_INSTRU;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_CTRL" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_TYPE , DBAL_FIELD_TYPE_DEF_SOURCE_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 1);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_START_TIME , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERVAL_PERIOD , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_instru.synced_counters.interval_period_size_hw_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_INTERVALS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_CONTINUOUS , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_DONE_STATE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CDU_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CGM_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CDB_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CGM_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_START_TIME + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDU_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDU_INST_START_TIMEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_START_TIME + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_START_TIMEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_START_TIME + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDB_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDB_INST_START_TIMEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_START_TIME + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_START_TIMEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDU_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDU_INST_INTERVAL_PERIODf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_INTERVAL_PERIODf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDB_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDB_INST_INTERVAL_PERIODf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_INTERVAL_PERIODf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDU_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDU_INST_NOF_INTERVALSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_NOF_INTERVALSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDB_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDB_INST_NOF_INTERVALSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_NOF_INTERVALSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDU_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDU_INST_OVERRIDE_MODEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_OVERRIDE_MODEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDB_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDB_INST_OVERRIDE_MODEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_OVERRIDE_MODEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDU_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDU_INST_START_AGAINf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_START_AGAINf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CDB_INSTRUMENTATION_CTRLr;
                        
                        fieldHwEntityId = CDB_INST_START_AGAINf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_SOURCE_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_CGM_INST_CTRLr;
                        
                        fieldHwEntityId = CGM_INST_START_AGAINf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_trigger_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL , is_valid , "INST_SYNCED_TRIGGER_CTRL" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_J2P_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_INSTRU;
    
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CGM_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CGM_ENABLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                    
                    fieldHwEntityId = CGM_INSTRUMENTATION_ENABLEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CDU_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CDU_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CDU_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                        
                        fieldHwEntityId = CDB_INSTRUMENTATION_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    }
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CRPS_ENABLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ECI_BROAD_VIEW_CTRLr;
                    
                    fieldHwEntityId = CRPS_INSTRUMENTATION_ENABLEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_setting_icgm_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM , is_valid , "INST_SYNCED_COUNTERS_SETTING_ICGM" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_J2P_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_INSTRU;
    
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_INIT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_CONTINUOUS , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_SETTINGSr;
                    
                    fieldHwEntityId = INSTRUMENTATION_INITf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_SETTINGSr;
                    
                    fieldHwEntityId = INSTRUMENTATION_INTERVAL_NUMf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_SETTINGSr;
                    
                    fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_setting_nif_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_NIF;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_NIF , is_valid , "INST_SYNCED_COUNTERS_SETTING_NIF" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_J2P_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_INSTRU;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_SETTING_NIF" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHU , DBAL_FIELD_TYPE_DEF_ETHU_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 2);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_INIT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_CONTINUOUS , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_NIF, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit) , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit)-1 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit) , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit)-1 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit) , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit)-1 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_RESET_LOWr;
                        
                        fieldHwEntityId = INSTRUMENTATION_RESET_LOWf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_NUMBER_OF_INTERVALS_USEDr;
                        
                        fieldHwEntityId = NUMBER_OF_INTERVALS_USEDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_OVERRIDE_MODEr;
                        
                        fieldHwEntityId = INSTRUMENTATION_OVERRIDE_MODE_CONFIGf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_NIF, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_last_count_icgn_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN , is_valid , "INST_SYNCED_COUNTERS_LAST_COUNT_ICGN" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_LAST_COUNT_ICGN" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 1);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_WRITE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_WRITES , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        cur_table_param->hl_access[map_idx].is_packed_fields = TRUE;
        
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_STATUSr;
                    
                    fieldHwEntityId = INSTRUMENTATION_LAST_WRITTEN_ADDRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_last_count_nif_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_NIF;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_NIF , is_valid , "INST_SYNCED_COUNTERS_LAST_COUNT_NIF" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_LAST_COUNT_NIF" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHU , DBAL_FIELD_TYPE_DEF_ETHU_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 2);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_WRITE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_WRITES , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_NIF, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit) , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit)-1 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit) , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , dnx_data_nif.eth.cdu_nof_get(unit)-1 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LAST_WRITE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_LAST_INTERVAL_WRITTENr;
                        
                        fieldHwEntityId = LAST_INTERVAL_WRITTENf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NOF_WRITES + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_INTERVALS_COUNTERr;
                        
                        fieldHwEntityId = INTERVALS_COUNTERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_NIF, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_icgm_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM , is_valid , "INST_SYNCED_COUNTERS_RECORDS_ICGM" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_RECORDS_ICGM" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTERS_RECORD_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 2);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 21);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_BUFFERS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_PDBS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_DRAM_BDBS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_SHARED_WORDS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_BUFFERS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_PDS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_HEADROOM_WORDS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_WORDS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_WORDS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_BUFFERS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_0_BUFFERS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_1_BUFFERS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_PDBS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_PDBS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_PDBS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_PACKETS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_BYTES , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_REJECT_PACKETS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_REJECT_BYTES , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_PACKETS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_BYTES , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
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
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_BUFFERS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = SRAM_BUFFERS_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_PDBS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = SRAM_PDBS_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_DRAM_BDBS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = DRAM_BDBS_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_SHARED_WORDS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = WORDS_MIN_FREE_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_BUFFERS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = SRAM_BUFFERS_MIN_FREE_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_PDS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_0m;
                    
                    fieldHwEntityId = SRAM_PDS_MIN_FREE_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_HEADROOM_WORDS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_2m;
                    
                    fieldHwEntityId = WORDS_HEADROOM_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_WORDS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_2m;
                    
                    fieldHwEntityId = WORDS_SHARED_POOL_0_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_WORDS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_2m;
                    
                    fieldHwEntityId = WORDS_SHARED_POOL_0_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_BUFFERS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_BUFFERS_HEADROOM_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_0_BUFFERS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_BUFFERS_SHARED_POOL_0_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_1_BUFFERS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_BUFFERS_SHARED_POOL_1_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_PDBS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_PDS_HEADROOM_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_PDBS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_PDS_SHARED_POOL_0_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_PDBS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_3m;
                    
                    fieldHwEntityId = SRAM_PDS_SHARED_POOL_1_FREE_MIN_STATUSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_PACKETS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_1m;
                    
                    fieldHwEntityId = ENQ_PKT_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_BYTES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_1m;
                    
                    fieldHwEntityId = ENQ_BYTES_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_REJECT_PACKETS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_1m;
                    
                    fieldHwEntityId = RJCJ_PKT_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_REJECT_BYTES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_1m;
                    
                    fieldHwEntityId = RJCT_BYTES_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_PACKETS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_2m;
                    
                    fieldHwEntityId = DEQ_PKT_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_BYTES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CGM_INSTRUMENTATION_MEMORY_2m;
                    
                    fieldHwEntityId = DEQ_BYTES_CTRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_nif_cdu_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CDU;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CDU , is_valid , "INST_SYNCED_COUNTERS_RECORDS_NIF_CDU" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_RECORDS_NIF_CDU" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTERS_RECORD_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHU , DBAL_FIELD_TYPE_DEF_ETHU_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PHYS_PORT_IN_ETHU , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.max_value = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 4);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 36);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CDU, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDB_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
            access_counter = 0;
            sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN , DBAL_FIELD_ETHU , 4 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN , DBAL_FIELD_ETHU , 3 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
                
                {
                    int instance_index = 0;
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_ETHU , 0 , (uint32) DBAL_DB_INVALID ));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDUM_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_cdu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CDU, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_nif_clu_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CLU;
    int is_valid = dnx_data_nif.eth.clu_nof_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CLU , is_valid , "INST_SYNCED_COUNTERS_RECORDS_NIF_CLU" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "INST_SYNCED_COUNTERS_RECORDS_NIF_CLU" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTERS_RECORD_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHU , DBAL_FIELD_TYPE_DEF_ETHU_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)+dnx_data_nif.eth.clu_nof_get(unit)-1;
            db_field.min_value = dnx_data_nif.eth.cdu_nof_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PHYS_PORT_IN_ETHU , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.max_value = dnx_data_nif.eth.nof_lanes_in_clu_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 4);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 36);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CLU, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        table_entry->core_mode = DBAL_CORE_MODE_DPC;
        
        dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
        
        {
            int map_idx = 0;
            int access_counter;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_RECEIVED + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_TDM + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_0 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_1 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_2 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PKT_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_BYTES_DROPPED_PRIORITY_3 + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_0_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_1_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_2_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_3_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_4_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_5_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*5; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*4; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_6_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*3; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_NOF_ON_TRANSITIONS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*2; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_MAX_CLOCKS_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*1; 
                        }
                    }
                }
            }
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_PFC_7_ON_CLOCK_CYCLES_DURATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CLU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm;
                        
                        fieldHwEntityId = DATAf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = inst_synced_counters_records_nif_clu_synced_counters_pkt_received_entryoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        {
                            access_params->data_offset.formula_int = 64*0; 
                        }
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_NIF_CLU, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_tod_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_TOD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INST_TOD , is_valid , "INST_TOD" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOD_NTP , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOD_1588 , DBAL_FIELD_TYPE_DEF_ARRAY8 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INST_TOD, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        
        cur_table_param->hl_access[map_idx].is_packed_fields = TRUE;
        
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ECI_GP_STATUS_0r;
                    
                    fieldHwEntityId = TODW_NTP_TIMESTAMPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_TOD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_trigger_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_setting_icgm_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_setting_nif_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_last_count_icgn_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_last_count_nif_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_icgm_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_nif_cdu_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_nif_clu_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_tod_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
