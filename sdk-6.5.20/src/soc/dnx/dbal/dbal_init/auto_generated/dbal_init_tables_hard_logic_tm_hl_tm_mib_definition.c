
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_STAT_CTRL;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_STAT_CTRL , is_valid , "FABRIC_STAT_CTRL" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "FABRIC_STAT_CTRL" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FMAC_ID , DBAL_FIELD_TYPE_DEF_FMAC_BLK_ID , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE_SELECT , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_SELECT , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_ON_READ , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_MODE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DATA_COUNTER_HEADER , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_STAT_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_J2P_A0)
    {
        
        
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
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = LANE_SELECTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_LANE_SELECT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = LANE_SELECTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_COUNTER_SELECT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = COUNTER_SELECTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CLEAR_ON_READ + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = COUNTER_CLEAR_ON_READf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_COUNTER_MODE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = DATA_COUNTER_MODEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_DATA_COUNTER_HEADER + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr;
                        
                        fieldHwEntityId = DATA_BYTE_COUNTER_HEADERf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_STAT_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_counters_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_STAT_COUNTERS;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_STAT_COUNTERS , is_valid , "FABRIC_STAT_COUNTERS" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "FABRIC_STAT_COUNTERS" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FMAC_ID , DBAL_FIELD_TYPE_DEF_FMAC_BLK_ID , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FMAC_COUNTER , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_STAT_COUNTERS, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_FMAC_COUNTER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = FMAC_FMAL_STATISTICS_OUTPUTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_STAT_COUNTERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_rx_part1_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART1;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART1 , is_valid , "NIF_CDU_MIB_COUNTERS_RX_PART1" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_RX_PART1" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 32);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R64 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R127 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R255 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R511 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R1023 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R1518 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMGV , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R2047 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R4095 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R9216 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R16383 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RBCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPOK , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RUCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_CDU_MIB_COUNTERS_RX_PART1_RESERVED0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXPF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXPP , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXCF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFCS , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RERPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFLR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJBR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMTUE , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ROVR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART1, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_R64 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R127 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R255 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R511 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R1023 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R1518 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMGV + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R2047 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R4095 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R9216 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R16383 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RBCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPOK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RUCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_NIF_CDU_MIB_COUNTERS_RX_PART1_RESERVED0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXPF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXPP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXCF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFCS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RERPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFLR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RJBR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMTUE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_ROVR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RDVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART1, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_rx_part2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART2;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART2 , is_valid , "NIF_CDU_MIB_COUNTERS_RX_PART2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_RX_PART2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 26);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXUO , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXUDA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXWSA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPRM , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RUND , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFRG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RRPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESERVED1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RRBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART2, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_RXUO + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXUDA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXWSA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPRM + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RUND + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFRG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RRPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RESERVED1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RRBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_tx_part1_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART1;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART1 , is_valid , "NIF_CDU_MIB_COUNTERS_TX_PART1" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_TX_PART1" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 24);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T64 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T127 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T255 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T511 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T1023 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T1518 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TMGV , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T2047 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T4095 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T9216 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T16383 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TBCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART1, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_T64 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T127 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T255 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T511 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T1023 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T1518 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TMGV + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T2047 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T4095 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T9216 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T16383 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TBCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART1, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_tx_part2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART2;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART2 , is_valid , "NIF_CDU_MIB_COUNTERS_TX_PART2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_TX_PART2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPOK , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TUCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TUFL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TMCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXPF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXPP , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXCF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TFCS , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TERR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOVR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TJBR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TFRG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TDVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART2, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPOK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TUCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TUFL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TMCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXPF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXPP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXCF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TFCS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 256; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TERR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 320; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TOVR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TJBR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 448; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TRPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TFRG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 64; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 128; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TDVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 192; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 384; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row0_2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW0_2;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW0_2 , is_valid , "NIF_CDU_MIB_COUNTERS_ROW0_2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_ROW0_2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW0_2, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_r4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rpkt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW0_2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row3_5_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW3_5;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW3_5 , is_valid , "NIF_CDU_MIB_COUNTERS_ROW3_5" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_ROW3_5" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW3_5, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part1_rfcs_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rxuo_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW3_5, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row6_8_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW6_8;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW6_8 , is_valid , "NIF_CDU_MIB_COUNTERS_ROW6_8" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_ROW6_8" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW8 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW6_8, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t64_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW8 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_t4095_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW6_8, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row9_11_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW9_11;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW9_11 , is_valid , "NIF_CDU_MIB_COUNTERS_ROW9_11" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_ROW9_11" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW9 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW10 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW11 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW9_11, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW9 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part1_tpfc2_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW10 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tpfc6_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW11 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_tx_part2_tmca_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW9_11, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row12_12_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW12_12;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW12_12 , is_valid , "NIF_CDU_MIB_COUNTERS_ROW12_12" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CDU_MIB_COUNTERS_ROW12_12" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_INDEX , DBAL_FIELD_TYPE_DEF_PM_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_cdu_pms_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_MIB_ROW12 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW12_12, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_CDU_MIB_ROW12 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CDMIB_MEMm;
                    
                    fieldHwEntityId = MIB_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = nif_cdu_mib_counters_rx_part2_rbyt_entryoffset_0_cb;
                    access_params->block_index.formula_cb = nif_cdu_mib_counters_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW12_12, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_rx_part1_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1 , is_valid , "NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 27);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R64 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R127 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R255 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R511 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R1023 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R1518 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMGV , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R2047 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R4095 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R9216 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_R16383 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RBCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPROG3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPOK , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RUCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXPF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXPP , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXCF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFCS , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RERPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFLR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJBR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_R64 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R64r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R127 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R127r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R255 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R255r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R511 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R511r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R1023 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R1023r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R1518 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R1518r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMGV + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RMGVr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R2047 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R2047r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R4095 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R4095r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R9216 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R9216r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_R16383 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_R16383r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RBCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RBCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPROG0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPROG1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPROG2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPROG3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPROG3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPKTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPOK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPOKr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RUCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RUCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RMCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXPF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXPFr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXPP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXPPr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXCF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXCFr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFCS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RFCSr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RERPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RERPKTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFLR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RFLRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RJBR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RJBRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_rx_part2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2 , is_valid , "NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 29);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMTUE , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ROVR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXUO , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXUDA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RXWSA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPRM , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFC7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPFCOFF7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RUND , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RFRG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RRPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RRBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMTUE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RMTUEr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_ROVR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_ROVRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RVLNr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RDVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RDVLNr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXUO + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXUOr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXUDA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXUDAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RXWSA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RXWSAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPRM + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPRMr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC4r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF4r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC5r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF5r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC6r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF6r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFC7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFC7r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RPFCOFF7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RPFCOFF7r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RUND + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RUNDr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RFRG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RFRGr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RRPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RRPKTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RBYTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RRBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RRBYTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_tx_part1_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1 , is_valid , "NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 21);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T64 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T127 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T255 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T511 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T1023 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T1518 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TMGV , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T2047 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T4095 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T9216 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_T16383 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TBCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_T64 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T64r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T127 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T127r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T255 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T255r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T511 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T511r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T1023 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T1023r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T1518 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T1518r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TMGV + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TMGVr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T2047 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T2047r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T4095 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T4095r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T9216 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T9216r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_T16383 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_T16383r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TBCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TBCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC4r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_tx_part2_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2 , is_valid , "NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 24);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF4 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF5 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF6 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFC7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPFCOFF7 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPOK , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TUCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TUFL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TMCA , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXPF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXPP , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXCF , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TFCS , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TERR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOVR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TJBR , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRPKT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TFRG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TDVLN , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TBYT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF4 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF4r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC5r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF5 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF5r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC6r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF6 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF6r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFC7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFC7r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPFCOFF7 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPFCOFF7r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPKTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TPOK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TPOKr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TUCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TUCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TUFL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TUFLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TMCA + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TMCAr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXPF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TXPFr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXPP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TXPPr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXCF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TXCFr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TFCS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TFCSr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TERR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TERRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TOVR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TOVRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TJBR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TJBRr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TRPKT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TRPKTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TFRG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TFRGr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TVLNr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TDVLN + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TDVLNr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TBYT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TBYTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_dedicated_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_MIB_COUNTERS_DEDICATED;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_DEDICATED , is_valid , "NIF_CLU_MIB_COUNTERS_DEDICATED" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_CLU_MIB_COUNTERS_DEDICATED" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_CLU_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 28);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RVERIFY , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RREPLY , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMCRC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RTRFU , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RSCHCRC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_EEE_LPI_EVENT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_EEE_LPI_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_LLFC_PHY , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_LLFC_LOG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_LLFC_CRC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_HCFC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_HCFC_CRC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TVERIFY , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TREPLY , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TSCL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TMCL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TLCL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TXCL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_EEE_LPI_EVENT , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_EEE_LPI_DURATION , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_LLFC_LOG , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_HCFC , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TNCL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_XTHOL , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.permission = DBAL_PERMISSION_TRIGGER;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TSPARE0 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TSPARE1 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TSPARE2 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TSPARE3 , DBAL_FIELD_TYPE_DEF_ARRAY32 , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_DEDICATED, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_RVERIFY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RVERIFYr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RREPLY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RREPLYr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RMCRC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RMCRCr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RTRFU + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RTRFUr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RSCHCRC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RSCHCRCr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_EEE_LPI_EVENT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_EEE_LPI_EVENT_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_EEE_LPI_DURATION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_EEE_LPI_DURATION_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_LLFC_PHY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_LLFC_PHY_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_LLFC_LOG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_LLFC_LOG_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_LLFC_CRC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_LLFC_CRC_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_HCFC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_HCFC_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_HCFC_CRC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_RX_HCFC_CRC_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TVERIFY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TVERIFYr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TREPLY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TREPLYr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TSCL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TSCLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TMCL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TMCLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TLCL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TLCLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TXCL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TXCLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_EEE_LPI_EVENT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TX_EEE_LPI_EVENT_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_EEE_LPI_DURATION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TX_EEE_LPI_DURATION_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_LLFC_LOG + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TX_LLFC_LOG_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_HCFC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TX_HCFC_COUNTERr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TNCL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TNCLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_XTHOL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_XTHOLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TSPARE0 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TSPARE0r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TSPARE1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TSPARE1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TSPARE2 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TSPARE2r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_TSPARE3 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = CLMIB_TSPARE3r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_clu_mib_counters_common_rx_part1_r64_blockindex_0_cb;
                    {
                        access_params->data_offset.formula_int = 0; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_DEDICATED, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_gtimer_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL , is_valid , "FABRIC_STAT_GTIMER_CTRL" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "FABRIC_STAT_GTIMER_CTRL" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FMAC_ID , DBAL_FIELD_TYPE_DEF_FMAC_BLK_ID , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GTIMER_PERIOD , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GTIMER_START , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_GTIMER_PERIOD + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = FMAC_FMAL_STATISTICS_GTIMERr;
                    
                    fieldHwEntityId = GTIMER_PERIODf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_GTIMER_START + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = FMAC_FMAL_STATISTICS_GTIMERr;
                    
                    fieldHwEntityId = GTIMER_STARTf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = fabric_stat_ctrl_null_blockindex_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_STAT_GTIMER_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_rx_stats_acc_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILKN_RX_STATS_ACC;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC , is_valid , "NIF_ILKN_RX_STATS_ACC" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_ILKN_RX_STATS_ACC" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOGICAL_PORT_TYPE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CMD , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADDR , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_LOGICAL_PORT_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_RX_STATS_ACC_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_CMD + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_RX_STATS_ACC_CMDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_ADDR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_RX_STATS_ACC_ADDRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_tx_stats_acc_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILKN_TX_STATS_ACC;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC , is_valid , "NIF_ILKN_TX_STATS_ACC" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_ILKN_TX_STATS_ACC" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOGICAL_PORT_TYPE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CMD , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADDR , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_LOGICAL_PORT_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_TX_STATS_ACC_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_CMD + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_TX_STATS_ACC_CMDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_ADDR + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_ACCr;
                    
                    fieldHwEntityId = SLE_TX_STATS_ACC_ADDRf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_stats_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILKN_STATS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILKN_STATS , is_valid , "NIF_ILKN_STATS" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_ILKN_STATS" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOGICAL_PORT , DBAL_FIELD_TYPE_DEF_LOGICAL_PORT , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 12);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_PACKETS_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_PACKETS_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_BYTES_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_BYTES_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_ERR_PACKETS_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_ERR_PACKETS_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_PACKETS_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_PACKETS_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_BYTES_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_BYTES_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_ERR_PACKETS_LOW , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_ERR_PACKETS_HIGH , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILKN_STATS, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_PACKETS_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_PKT_LOWr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_PKT_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_PACKETS_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_PKT_HIGHr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_PKT_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_BYTES_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_BYTE_LOWr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_BYTE_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_BYTES_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_BYTE_HIGHr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_BYTE_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_ERR_PACKETS_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_ERR_LOWr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_ERR_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_ERR_PACKETS_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_RX_STATS_RD_ERR_HIGHr;
                    
                    fieldHwEntityId = SLE_RX_STATS_RD_ERR_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_PACKETS_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_PKT_LOWr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_PKT_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_PACKETS_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_PKT_HIGHr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_PKT_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_BYTES_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_BYTE_LOWr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_BYTE_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_BYTES_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_BYTE_HIGHr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_BYTE_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_ERR_PACKETS_LOW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_ERR_LOWr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_ERR_LOW_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_ERR_PACKETS_HIGH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ILKN_SLE_TX_STATS_RD_ERR_HIGHr;
                    
                    fieldHwEntityId = SLE_TX_STATS_RD_ERR_HIGH_RDATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->block_index.formula_cb = nif_ilkn_rx_stats_acc_logical_port_type_blockindex_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILKN_STATS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_elk_fec_counters_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS;
    int is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_fec_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS , is_valid , "NIF_ILKN_ELK_FEC_COUNTERS" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_MIB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "NIF_ILKN_ELK_FEC_COUNTERS" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID , DBAL_FIELD_TYPE_DEF_CORE_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_UNIT , DBAL_FIELD_TYPE_DEF_ILKN_FEC_UNIT_ID , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_FEC_CORRECTED_CNT , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_FEC_UNCORRECTED_CNT , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_RX_FEC_CORRECTED_CNT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ILE_RX_FEC_DECODER_CORRECTED_CODE_WORD_CNTr;
                        
                        fieldHwEntityId = RX_FEC_N_CORRECTED_CODE_WORD_CNTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->array_offset.formula_cb = nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_RX_FEC_CORRECTED_CNT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_RX_FEC_DECODER_CORRECTED_CODE_WORD_CNTr;
                        
                        fieldHwEntityId = RX_FEC_N_CORRECTED_CODE_WORD_CNTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->array_offset.formula_cb = nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_1_cb;
                        access_params->block_index.formula_cb = nif_ilkn_elk_fec_counters_rx_fec_corrected_cnt_blockindex_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_RX_FEC_UNCORRECTED_CNT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ILE_RX_FEC_DECODER_UNCORRECTED_CODE_WORD_CNTr;
                        
                        fieldHwEntityId = RX_FEC_N_UNCORRECTED_CODE_WORD_CNTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->array_offset.formula_cb = nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_RX_FEC_UNCORRECTED_CNT + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = CDU_RX_FEC_DECODER_UNCORRECTED_CODE_WORD_CNTr;
                        
                        fieldHwEntityId = RX_FEC_N_UNCORRECTED_CODE_WORD_CNTf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->array_offset.formula_cb = nif_ilkn_fec_enable_fec_tx_serdes_en_arrayoffset_1_cb;
                        access_params->block_index.formula_cb = nif_ilkn_elk_fec_counters_rx_fec_corrected_cnt_blockindex_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILKN_ELK_FEC_COUNTERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_counters_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_rx_part1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_rx_part2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_tx_part1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_tx_part2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row0_2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row3_5_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row6_8_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row9_11_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_cdu_mib_counters_row12_12_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_rx_part1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_rx_part2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_tx_part1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_common_tx_part2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_clu_mib_counters_dedicated_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_fabric_stat_gtimer_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_rx_stats_acc_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_tx_stats_acc_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_stats_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_nif_ilkn_elk_fec_counters_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
