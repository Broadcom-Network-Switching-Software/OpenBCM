
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_profile_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO , is_valid , "QOS_INGRESS_PHB_PROFILE_INFO" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_PHB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_PHB_PROFILE_INFO" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TCDP_PROFILE_RESOLUTION_MEMm;
                    
                    fieldHwEntityId = PROGRAM_SELECTION_PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_profile_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO , is_valid , "QOS_INGRESS_REMARK_PROFILE_INFO" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_REMARK_PROFILE_INFO" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_PROFILE_RESOLUTION_MEMm;
                    
                    fieldHwEntityId = PROGRAM_SELECTION_PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_profile_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_INFO , is_valid , "QOS_INGRESS_ECN_PROFILE_INFO" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_ECN;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_ECN_PROFILE_INFO" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_INFO, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_PROFILE_RESOLUTION_MEMm;
                    
                    fieldHwEntityId = PROGRAM_SELECTION_PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_profile_to_internal_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID , is_valid , "QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_PHB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_QOS_MAP_INDEX , DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_MAP_INDEX_EXPLICIT_VALUE_MEMm;
                        
                        fieldHwEntityId = MAP_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_MAP_INDEX_EXPLICIT_VALUE_MEMm;
                        
                        fieldHwEntityId = MAP_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_phb_profile_to_internal_map_id_qos_int_map_id_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_phb_profile_to_internal_map_id_qos_int_map_id_dataoffset_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_profile_to_inter_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID , is_valid , "QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_QOS_MAP_INDEX , DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_REMARK_MAP_INDEX_EXPLICIT_VALUE_MEMm;
                        
                        fieldHwEntityId = MAP_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_REMARK_MAP_INDEX_EXPLICIT_VALUE_MEMm;
                        
                        fieldHwEntityId = MAP_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_phb_profile_to_internal_map_id_qos_int_map_id_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_remark_profile_to_inter_map_id_qos_int_map_id_dataoffset_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_REMARK_PROFILE_TO_INTER_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_profile_to_internal_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID , is_valid , "QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_ECN;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_QOS_MAP_INDEX , DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_MAP_INDEX_EXPLICIT_VALUE_MEMm;
                    
                    fieldHwEntityId = MAP_INDEXf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_ECN_PROFILE_TO_INTERNAL_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_internal_map_entries_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES , is_valid , "QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_PHB;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_KEY , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC , DBAL_FIELD_TYPE_DEF_TC , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
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
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_TC + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_RESULT_MEMm;
                        
                        fieldHwEntityId = TCf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_phb_internal_map_entries_tc_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_phb_internal_map_entries_tc_dataoffset_0_cb;
                    }
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_DP + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_RESULT_MEMm;
                        
                        fieldHwEntityId = DPf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_phb_internal_map_entries_tc_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_phb_internal_map_entries_tc_dataoffset_0_cb;
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_TC + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_RESULT_MEMm;
                        
                        fieldHwEntityId = TCf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_DP + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_TC_DP_RESULT_MEMm;
                        
                        fieldHwEntityId = DPf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
                }
            }
            
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
            
            map_idx++;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_internal_map_entries_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES , is_valid , "QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_KEY , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NETWORK_QOS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_REMARK_RESULT_MEMm;
                        
                        fieldHwEntityId = NETWORK_QOSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_NETWORK_QOS + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_REMARK_RESULT_MEMm;
                        
                        fieldHwEntityId = NETWORK_QOSf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_phb_internal_map_entries_tc_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_remark_internal_map_entries_network_qos_dataoffset_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_REMARK_INTERNAL_MAP_ENTRIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_internal_map_entries_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES , is_valid , "QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_ECN;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_KEY , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_VALUE , DBAL_FIELD_TYPE_DEF_QOS_ECN , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ECN_VALUE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_ECN_RESULT_MEMm;
                        
                        fieldHwEntityId = ECNf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ECN_VALUE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_PHB_ECN_RESULT_MEMm;
                        
                        fieldHwEntityId = ECNf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = qos_ingress_ecn_internal_map_entries_ecn_value_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = qos_ingress_ecn_internal_map_entries_ecn_value_dataoffset_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_ECN_INTERNAL_MAP_ENTRIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_init_context_selection_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION , is_valid , "QOS_INGRESS_INIT_CONTEXT_SELECTION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 5;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_PHB;
    table_entry->table_labels[2] = DBAL_LABEL_QOS_REMARKING;
    table_entry->table_labels[3] = DBAL_LABEL_QOS_ECN;
    table_entry->table_labels[4] = DBAL_LABEL_QOS_TTL;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_INIT_CONTEXT_SELECTION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CAM_PRIORITY , DBAL_FIELD_TYPE_DEF_QOS_CAM_PRIORITY , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INGRESS_APP_TYPE , DBAL_FIELD_TYPE_DEF_QOS_INGRESS_APP_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 15 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_TYPES , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_TYPE , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_QUALIFIER , DBAL_FIELD_TYPE_DEF_LAYER_QUALIFIER , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_QUALIFIER , DBAL_FIELD_TYPE_DEF_NEXT_LAYER_QUALIFIER , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_TYPE_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_TYPE_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_QUALIFIER_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CS_PROFILE_MASK , DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE_MASK , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_ACTION , DBAL_FIELD_TYPE_DEF_QOS_ACTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_QOS_MAP_INDEX , DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FFC_INSTRUCTION , DBAL_FIELD_TYPE_DEF_FFC_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_RESOLUTION_MODE , DBAL_FIELD_TYPE_DEF_QOS_RESOLUTION_MODE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_KEY_SIZE , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY_SIZE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION, is_standard_1, is_compatible_with_all_images));
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
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_CS_PROFILE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = PROFILE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_ACTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = ACTIONf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_ACTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = ACTIONf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_ACTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = ACTIONf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_INGRESS_QOS_MAP_INDEX + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = CONTEXTf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_INGRESS_QOS_MAP_INDEX + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = CONTEXTf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_INGRESS_QOS_MAP_INDEX + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = CONTEXTf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                instance_index = 0;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 1;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 0;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 1;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 0;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 1;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_RESOLUTION_MODE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_RESOLUTION_MODE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_RESOLUTION_MODE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_KEY_SIZE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = MAP_SIZEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_KEY_SIZE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = MAP_SIZEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_KEY_SIZE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = MAP_SIZEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = VALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = VALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_QOS_INGRESS_APP_TYPE , 0 , (uint32) DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_REMARK_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = VALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_INIT_CONTEXT_SELECTION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_initial_pcp_dei_map_type_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE , is_valid , "QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LLVP_PCP_DEI_MAP_TYPE , DBAL_FIELD_TYPE_DEF_PCP_DEI_MAP_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PCP_DEI_MAP_EXTEND , DBAL_FIELD_TYPE_DEF_PCP_DEI_MAP_TYPE , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INITIAL_PCP_DEI_MAP_TYPE , DBAL_FIELD_TYPE_DEF_PCP_DEI_MAP_TYPE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_INITIAL_PCP_DEI_MAP_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_INITIAL_PCP_DEI_MAP_TYPE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_IVE_INITIAL_PCP_DEI_MAP_TYPE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_init_ttl_context_selection_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION , is_valid , "QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_TTL;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_CAM_PRIORITY , DBAL_FIELD_TYPE_DEF_QOS_CAM_PRIORITY , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 10 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_TYPES , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_TYPE , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_QUALIFIER , DBAL_FIELD_TYPE_DEF_LAYER_QUALIFIER , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_QUALIFIER , DBAL_FIELD_TYPE_DEF_NEXT_LAYER_QUALIFIER , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_TYPE_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_TYPE_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_QUALIFIER_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_ACTION , DBAL_FIELD_TYPE_DEF_QOS_ACTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FFC_INSTRUCTION , DBAL_FIELD_TYPE_DEF_FFC_INSTRUCTION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPES + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIERf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_TYPE_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_TYPE_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_QUALIFIER_MASK + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = NEXT_LAYER_QUALIFIER_MASKf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_ACTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = ACTIONf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
        }
        
        {
            table_db_access_params_struct_t * access_params;
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 0;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                instance_index = 1;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_FFC_INSTRUCTION + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_ACTION_MEMm;
                    
                    fieldHwEntityId = FFC_INSTRUCTION_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_CONTEXT_SELECTION_CAMm;
                    
                    fieldHwEntityId = VALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_init_context_selection_layer_types_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_INIT_TTL_CONTEXT_SELECTION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_pcp_dei_explicit_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP , is_valid , "QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 3 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_PCP_DEI_QOS_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 3);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUT_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUT_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP, is_standard_1, is_compatible_with_all_images));
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
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_EVEN , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 1 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_EVEN , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_ODD , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_2f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 1 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_ODD , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_3f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_EVEN , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_0f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                    {
                        
                        uint32 result_size__out_dei = 0;
                        
                        result_size__out_dei += 1;
                        access_params->data_offset.formula_int = result_size__out_dei; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 1 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_EVEN , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_1f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                    {
                        
                        uint32 result_size__out_dei = 0;
                        
                        result_size__out_dei += 1;
                        access_params->data_offset.formula_int = result_size__out_dei; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_ODD , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_2f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                    {
                        
                        uint32 result_size__out_dei = 0;
                        
                        result_size__out_dei += 1;
                        access_params->data_offset.formula_int = result_size__out_dei; 
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO , DBAL_FIELD_IN_DEI , 1 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_IS_ODD , DBAL_FIELD_IN_PCP , 0 , (uint32) DBAL_DB_INVALID ));
                condition_index++;
                access_params->nof_conditions = condition_index;
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PCP_DEI_MAP_PROFILE_3f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = qos_ingress_ive_pcp_dei_explicit_map_out_dei_entryoffset_0_cb;
                    {
                        
                        uint32 result_size__out_dei = 0;
                        
                        result_size__out_dei += 1;
                        access_params->data_offset.formula_int = result_size__out_dei; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_IVE_PCP_DEI_EXPLICIT_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_untag_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP , is_valid , "QOS_INGRESS_IVE_UNTAG_MAP" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_IVE_UNTAG_MAP" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC , DBAL_FIELD_TYPE_DEF_TC , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PADDING , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VLAN_PCP_DEI_QOS_PROFILE , DBAL_FIELD_TYPE_DEF_PCP_DEI_QOS_PROFILE , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUTER_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_VLAN_EDIT_PCP_DEI_MEMm;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_ingress_ive_untag_map_outer_dei_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUTER_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_VLAN_EDIT_PCP_DEI_MEMm;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_ingress_ive_untag_map_outer_pcp_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_INNER_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_VLAN_EDIT_PCP_DEI_MEMm;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_ingress_ive_untag_map_inner_dei_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_INNER_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_VLAN_EDIT_PCP_DEI_MEMm;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_ingress_ive_untag_map_inner_pcp_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_IVE_UNTAG_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_propag_profile_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING , is_valid , "QOS_INGRESS_PROPAG_PROFILE_MAPPING" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_PROPAG_PROFILE_MAPPING" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PROPAGATION_PROF , DBAL_FIELD_TYPE_DEF_PROPAGATION_PROF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INGRESS_APP_TYPE , DBAL_FIELD_TYPE_DEF_QOS_INGRESS_APP_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROPAG_STRENGTH , DBAL_FIELD_TYPE_DEF_QOS_PROPAG_STRENGTH , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_COMP_STRENGTH , DBAL_FIELD_TYPE_DEF_QOS_PROPAG_STRENGTH , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_PROPAG_STRENGTH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VTT_QOS_PROPAG_PROFILE_MAPPING , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_ingress_propag_profile_mapping_qos_propag_strength_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_COMP_STRENGTH + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_INGRESS_VTT_QOS_PROPAG_PROFILE_MAPPING , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_ingress_propag_profile_mapping_qos_comp_strength_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_PROPAG_PROFILE_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_decrement_configuration_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_TTL_DECREMENT_CONFIGURATION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_TTL_DECREMENT_CONFIGURATION , is_valid , "QOS_EGRESS_TTL_DECREMENT_CONFIGURATION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_TTL_DECREMENT_CONFIGURATION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO , DBAL_FIELD_TYPE_DEF_FORWARDING_ADDITIONAL_INFO , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_TYPES , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_TTL_DECREMENT_DISABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_TTL_DECREMENT_CONFIGURATION, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_TTL_DECREMENT_DISABLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_FWD_EGRESS_DO_NOT_TTL_DECREMENTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_TTL_DECREMENT_CONFIGURATION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_pipe_profile_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE , is_valid , "QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_PIPE_PROFILE , DBAL_FIELD_TYPE_DEF_NETWORK_QOS_PIPE_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_PIPE , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_PIPE_MAPPED , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP_PIPE_MAPPED , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_NETWORK_QOS_PIPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PIPE_NWK_QOS_VAR_O_2f;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_NWK_QOS_IDX_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NETWORK_QOS_PIPE_MAPPED + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PIPE_MAPPED_QOS_DPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_NWK_QOS_IDX_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_DP_PIPE_MAPPED + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PIPE_MAPPED_QOS_DPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_NWK_QOS_IDX_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                    {
                        
                        uint32 result_size__network_qos_pipe_mapped = 0;
                        
                        result_size__network_qos_pipe_mapped += 8;
                        access_params->data_offset.formula_int = result_size__network_qos_pipe_mapped; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_initial_select_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT , is_valid , "QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 3 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_QOS_MAP_TYPE_PLUS1 , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_QOS_MAP_TYPE , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_QOS_NETWORK_VALID , DBAL_FIELD_TYPE_DEF_INGRESS_QOS_NETWORK_VALID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 3);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT , DBAL_FIELD_TYPE_DEF_NETWORK_QOS_INITIAL_SELECT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT_PLUS_1 , DBAL_FIELD_TYPE_DEF_NETWORK_QOS_INITIAL_SELECT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_INITIAL_NWK_QOS_SELECTr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_network_qos_initial_select_network_qos_initial_select_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT_PLUS_1 + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_INITIAL_NWK_QOS_SELECT_PLUS_1r;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_network_qos_initial_select_network_qos_initial_select_plus_1_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_jer_ingress_nwk_qos_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING , is_valid , "QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_NWK_QOS_TYPE , DBAL_FIELD_TYPE_DEF_INGRESS_NWK_QOS_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_DSCP_EXP , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_JERICHO_INGRESS_NWK_QOS_TABLEm;
                    
                    fieldHwEntityId = NWK_QOSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_tc_nwk_qos_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING , is_valid , "QOS_EGRESS_TC_NWK_QOS_MAPPING" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_TC_NWK_QOS_MAPPING" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_TC , DBAL_FIELD_TYPE_DEF_TC , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC_NWK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TC_NWK_QOS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_CFG_TC_2_NWK_QOS_MAPPINGr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_tc_nwk_qos_mapping_tc_nwk_qos_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_nwk_qos_type_select_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_NWK_QOS_TYPE_SELECT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_NWK_QOS_TYPE_SELECT , is_valid , "QOS_INGRESS_NWK_QOS_TYPE_SELECT" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_INGRESS_NWK_QOS_TYPE_SELECT" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_CODE , DBAL_FIELD_TYPE_DEF_EGRESS_FWD_CODE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_NWK_QOS_TYPE , DBAL_FIELD_TYPE_DEF_INGRESS_NWK_QOS_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_NWK_QOS_TYPE_SELECT, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_INGRESS_NWK_QOS_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_CFG_INGRESS_NWK_QOS_TYPEr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_ingress_nwk_qos_type_select_ingress_nwk_qos_type_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_NWK_QOS_TYPE_SELECT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_fwd_layer_type_select_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT , is_valid , "QOS_FWD_LAYER_TYPE_SELECT" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_FWD_LAYER_TYPE_SELECT" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_PROTOCOL , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_TYPE , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_LAYER_PROTOCOL , DBAL_FIELD_TYPE_DEF_QOS_LAYER_PROTOCOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_LAYER_PROTOCOL_INDEX , DBAL_FIELD_TYPE_DEF_QOS_LR_FORWARD_P1_TYPE_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_FORWARD_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_CFG_MAP_FWD_LAYER_TYPEr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_fwd_layer_type_select_forward_type_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_LAYER_PROTOCOL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_FWD_LAYER_PROTOCOL , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_fwd_layer_type_select_qos_layer_protocol_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_FWD_PLUS_1_LAYER_PROTOCOL_INDEX + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_MAP_FWD_PLUS_1_PROTOCOLr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_fwd_layer_type_select_fwd_plus_1_layer_protocol_index_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_model_mapping_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE , is_valid , "QOS_EGRESS_MODEL_MAPPING_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_MODEL_MAPPING_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENCAP_QOS_MODEL , DBAL_FIELD_TYPE_DEF_ENCAP_QOS_MODEL , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_DP_SELECT , DBAL_FIELD_TYPE_DEF_QOS_DP_SELECT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_IS_ELIGIBLE , DBAL_FIELD_TYPE_DEF_ECN_IS_ELIGIBLE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP_IS_PRESERVE , DBAL_FIELD_TYPE_DEF_DP_IS_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS_IS_PRESERVE , DBAL_FIELD_TYPE_DEF_NETWORK_QOS_IS_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_MODEL_TYPE , DBAL_FIELD_TYPE_DEF_MODEL_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_EXPLICIT_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_DP_SELECT + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_MODEL_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_qos_dp_select_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_ECN_IS_ELIGIBLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_MODEL_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_ecn_is_eligible_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_DP_IS_PRESERVE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_MODEL_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_dp_is_preserve_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NETWORK_QOS_IS_PRESERVE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_MODEL_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_network_qos_is_preserve_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_MODEL_TYPE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_QOS_MODEL_IS_PIPEr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_qos_model_type_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_FWD_PLUS_1_EXPLICIT_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_MAP_FWD_PLUS_1_EXPLICIT_REMARK_PROFILEr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_model_mapping_table_fwd_plus_1_explicit_profile_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_map_type_select_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT , is_valid , "QOS_EGRESS_MAP_TYPE_SELECT" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_MAP_TYPE_SELECT" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FTMH_ECN_ELIGIBLE , DBAL_FIELD_TYPE_DEF_FTMH_ECN_ELIGIBLE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_CODE , DBAL_FIELD_TYPE_DEF_EGRESS_FWD_CODE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETPP_QOS_VAR_TYPE , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_IS_ELIGIBLE , DBAL_FIELD_TYPE_DEF_ECN_IS_ELIGIBLE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KEEP_ECN_BITS , DBAL_FIELD_TYPE_DEF_KEEP_ECN_BITS , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_MAP_FWD_QOS_DP_TO_TYPE_FWDm;
                    
                    fieldHwEntityId = TYPE_FWD_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_protocol_profile_to_internal_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID , is_valid , "QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_LAYER_PROTOCOL , DBAL_FIELD_TYPE_DEF_QOS_LAYER_PROTOCOL , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = CFG_REMARK_PROTOCOL_PROFILE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_REMARK_PROTOCOL_PROFILE , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_protocol_profile_to_internal_map_id_initial_network_qos_int_map_id_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_mapping_initial_network_qos_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS , is_valid , "QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PACKET_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INITIAL_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_INITIAL_NETWORK_QOS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = LAYER_NWK_QOSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_LAYER_NWK_QOS_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_pcp_dei_dp_mapping_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE , is_valid , "QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP_PROFILE , DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUT_DEI , DBAL_FIELD_TYPE_DEF_DEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUT_PCP , DBAL_FIELD_TYPE_DEF_PCP , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_DEI + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PUSH_PCP_DEIf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EGRESS_PCP_DEI_DP_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_OUT_PCP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PUSH_PCP_DEIf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EGRESS_PCP_DEI_DP_MAPPING_TABLE , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    {
                        
                        uint32 result_size__out_dei = 0;
                        
                        result_size__out_dei += 1;
                        access_params->data_offset.formula_int = result_size__out_dei; 
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_PCP_DEI_DP_MAPPING_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_profile_to_internal_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID , is_valid , "QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETPP_QOS_VAR_TYPE , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PROTOCOL_QOS_MAP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_FWD_MAP_PROTOCOL_QOS , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    {
                        
                        uint32 result_size__remark_or_preserve = 0;
                        
                        result_size__remark_or_preserve += 1;
                        access_params->data_offset.formula_int = result_size__remark_or_preserve; 
                    }
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_REMARK_OR_PRESERVE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = PROTOCOL_QOS_MAP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_FWD_MAP_PROTOCOL_QOS , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_fwd_header_remark_enabler_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER;
    int is_valid = dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_egress_remark_or_keep_enabler);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER , is_valid , "QOS_EGRESS_FWD_HEADER_REMARK_ENABLER" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_FWD_HEADER_REMARK_ENABLER" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_PROFILE_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARD_CODE_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    
    {
        int map_idx = 0;
        int access_counter;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY , sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_REMARK_OR_PRESERVE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ETPPC_RESERVED_SPARE_2r;
                        
                        fieldHwEntityId = RESERVED_SPARE_2f;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->data_offset.formula_cb = qos_egress_fwd_header_remark_enabler_remark_or_preserve_dataoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_REMARK_OR_PRESERVE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ETPPC_MAP_FWD_QOS_REMARK_OR_KEEPr;
                        
                        fieldHwEntityId = MAP_FWD_QOS_REMARK_OR_KEEPf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->data_offset.formula_cb = qos_egress_fwd_header_remark_enabler_remark_or_preserve_dataoffset_0_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_mapping_packet_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET , is_valid , "QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 3 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 3);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = NEW_QOS_VAR_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_NEW_QOS_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = vsi_learning_profile_vsi_learning_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_egress_network_qos_mapping_packet_current_layer_network_qos_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_mapping_next_layer_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER , is_valid , "QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 3 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 3);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_NETWORK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_LAYER_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_NETWORK_QOS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = NEW_QOS_DP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_DP_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = vsi_learning_profile_vsi_learning_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_egress_network_qos_mapping_next_layer_next_layer_network_qos_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NEXT_LAYER_DP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = NEW_QOS_DP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_QOS_DP_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = vsi_learning_profile_vsi_learning_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = qos_egress_network_qos_mapping_next_layer_next_layer_dp_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_remark_profile_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_REMARK_PROFILE_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_REMARK_PROFILE_INFO , is_valid , "QOS_EGRESS_REMARK_PROFILE_INFO" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_REMARK_PROFILE_INFO" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAPPED_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_REMARK_PROFILE_INFO, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_MAPPED_REMARK_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_REMARK_PROFILE_QOS_DP , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_remark_profile_info_mapped_remark_profile_dataoffset_0_cb;
                }
            }
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_MAPPED_REMARK_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_REMARK_PROFILE_QOS_VAR , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_remark_profile_info_mapped_remark_profile_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_REMARK_PROFILE_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_pipe_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING , is_valid , "QOS_EGRESS_TTL_PIPE_MAPPING" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_TTL;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_TTL_PIPE_MAPPING" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL_PIPE_PROFILE , DBAL_FIELD_TYPE_DEF_TTL_PIPE_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL , DBAL_FIELD_TYPE_DEF_TTL , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TTL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_PIPE_MAPPED_TTL_VARIABLE_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_ttl_pipe_mapping_ttl_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_model_configuration_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_TTL_MODEL_CONFIGURATION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_TTL_MODEL_CONFIGURATION , is_valid , "QOS_EGRESS_TTL_MODEL_CONFIGURATION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_TTL;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_TTL_MODEL_CONFIGURATION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_TTL_MODE , DBAL_FIELD_TYPE_DEF_TTL_MODE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL_DECREMENT_DISABLE , DBAL_FIELD_TYPE_DEF_TTL_DECREMENT_DISABLE , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL_INHERITANCE , DBAL_FIELD_TYPE_DEF_TTL_INHERITANCE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL_PIPE_MAP , DBAL_FIELD_TYPE_DEF_TTL_PIPE_MAP , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_TTL_MODEL_CONFIGURATION, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TTL_INHERITANCE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_IS_TTL_MODEL_INHERIT_PHPr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_ttl_model_configuration_ttl_inheritance_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_TTL_PIPE_MAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_CFG_TTL_MODEL_PIPE_MAP , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_ttl_model_configuration_ttl_inheritance_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_TTL_MODEL_CONFIGURATION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_inheritance_resolution_mpls_type_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE , is_valid , "QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_MPLS;
    
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PHB_INHERITANCE_RESOLUTION_MPLS , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_INHERITANCE_RESOLUTION_MPLS , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_INHERITANCE_RESOLUTION_MPLS , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL_INHERITANCE_RESOLUTION_MPLS , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_PHB_INHERITANCE_RESOLUTION_MPLS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_TC_DP_INHERITANCE_RESOLUTION_MPLS_CFGr;
                    
                    fieldHwEntityId = PHB_TC_DP_INHERITANCE_RESOLUTION_MPLS_CFGf;
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
                
                
                access_params->access_field_id = DBAL_FIELD_REMARK_INHERITANCE_RESOLUTION_MPLS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_DSCP_REMARK_INHERITANCE_RESOLUTION_MPLS_CFGr;
                    
                    fieldHwEntityId = DSCP_REMARK_INHERITANCE_RESOLUTION_MPLS_CFGf;
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
                
                
                access_params->access_field_id = DBAL_FIELD_ECN_INHERITANCE_RESOLUTION_MPLS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PHB_ECN_INHERITANCE_RESOLUTION_MPLS_CFGr;
                    
                    fieldHwEntityId = PHB_ECN_INHERITANCE_RESOLUTION_MPLS_CFGf;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TTL_INHERITANCE_RESOLUTION_MPLS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_TTL_INHERITANCE_RESOLUTION_MPLS_CFGr;
                    
                    fieldHwEntityId = TTL_INHERITANCE_RESOLUTION_MPLS_CFGf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_INGRESS_INHERITANCE_RESOLUTION_MPLS_TYPE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_policer_offset_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP , is_valid , "QOS_EGRESS_POLICER_OFFSET_MAP" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_POLICER_OFFSET_MAP" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC , DBAL_FIELD_TYPE_DEF_TC , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_POLICER_PROFILE_ID , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POLICER_OFFSET , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_POLICER_OFFSET + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_METER_OFFSET_MAPr;
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_policer_offset_map_policer_offset_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_jr1_fhei_model_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_JR1_FHEI_MODEL_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_JR1_FHEI_MODEL_MAP , is_valid , "QOS_EGRESS_JR1_FHEI_MODEL_MAP" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_JR1_FHEI_MODEL_MAP" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FHEI_MODEL , DBAL_FIELD_TYPE_DEF_MODEL_TYPE , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FHEI_QOS_MODEL , DBAL_FIELD_TYPE_DEF_MODEL_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FHEI_TTL_MODEL , DBAL_FIELD_TYPE_DEF_MODEL_TYPE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_JR1_FHEI_MODEL_MAP, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_FHEI_QOS_MODEL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_FHEI_MODEL , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_jr1_fhei_model_map_fhei_qos_model_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_FHEI_TTL_MODEL + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = INVALIDf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_FHEI_MODEL , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_jr1_fhei_model_map_fhei_ttl_model_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_JR1_FHEI_MODEL_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_enc_qos_var_type_mapping_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE;
    int is_valid = dnx_data_qos.qos.feature_get(unit, dnx_data_qos_qos_qos_var_type_map_enable);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE , is_valid , "ENC_QOS_VAR_TYPE_MAPPING_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "ENC_QOS_VAR_TYPE_MAPPING_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAYER_IS_ECN_ELIGIBLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_VAR_TYPE_CURRENT , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENC_QOS_VAR_TYPE_NEW , DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_ENC_QOS_VAR_TYPE_NEW + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    
                    fieldHwEntityId = QOS_VAR_TYPE_NEWf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_ETPP_ENC_QOS_VAR_TYPE_MAPPING , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_fwd_plus_1_remakr_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE , is_valid , "QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_EXPLICIT_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FTMH_ECN_ELIGIBLE , DBAL_FIELD_TYPE_DEF_FTMH_ECN_ELIGIBLE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_REMARK_INGRESS_PROFILE , DBAL_FIELD_TYPE_DEF_QOS_FORWARD_P1_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_LAYER_PROTOCOL_INDEX , DBAL_FIELD_TYPE_DEF_QOS_LR_FORWARD_P1_TYPE_INDEX , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE , DBAL_FIELD_TYPE_DEF_QOS_TYPEFWDPLUS1_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_IS_ELIGIBLE , DBAL_FIELD_TYPE_DEF_ECN_IS_ELIGIBLE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_REMARK_EXPLICIT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_NWK_QOS_REMARK_BMP , DBAL_FIELD_TYPE_DEF_BITMAP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = FALSE;
        
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
                
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_FWD_PLUS_1_REMARKING_TABLEm;
                    
                    fieldHwEntityId = FWD_PLUS_1_TYPEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_profile_to_fwd_plus_1_internal_map_id_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID , is_valid , "QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE , DBAL_FIELD_TYPE_DEF_QOS_TYPEFWDPLUS1_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_PROFILE , DBAL_FIELD_TYPE_DEF_REMARK_PROFILE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QOS_INT_MAP_ID , DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REMARK_OR_PRESERVE , DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_QOS_INT_MAP_ID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_TERM_MAP_PROTOCOL_QOSm;
                    
                    fieldHwEntityId = PROTOCOL_QOS_MAP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    {
                        
                        uint32 result_size__remark_or_preserve = 0;
                        
                        result_size__remark_or_preserve += 1;
                        access_params->data_offset.formula_int = result_size__remark_or_preserve; 
                    }
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_REMARK_OR_PRESERVE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_TERM_MAP_PROTOCOL_QOSm;
                    
                    fieldHwEntityId = PROTOCOL_QOS_MAP_DATAf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_cos_profile_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE , is_valid , "QOS_EGRESS_COS_PROFILE_TABLE" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 2;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_QOS;
    table_entry->table_labels[1] = DBAL_LABEL_QOS_REMARKING;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "QOS_EGRESS_COS_PROFILE_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COS_PROFILE , DBAL_FIELD_TYPE_DEF_COS_PROFILE , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC_VALID , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_TC , DBAL_FIELD_TYPE_DEF_TC , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP_VALID , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DP , DBAL_FIELD_TYPE_DEF_DP , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NWK_QOS_VALID , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_NWK_QOS , DBAL_FIELD_TYPE_DEF_NETWORK_QOS , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TC_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_tc_valid_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_EGRESS_TC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_egress_tc_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_DP_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_dp_valid_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_EGRESS_DP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_egress_dp_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_NWK_QOS_VALID + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_nwk_qos_valid_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_EGRESS_NWK_QOS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPA_COS_PROFILE_TABLEr;
                    
                    fieldHwEntityId = COS_PROFILE_TABLE_VECTORf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = qos_egress_cos_profile_table_egress_nwk_qos_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_profile_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_profile_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_profile_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_profile_to_internal_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_profile_to_inter_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_profile_to_internal_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_phb_internal_map_entries_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_remark_internal_map_entries_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ecn_internal_map_entries_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_init_context_selection_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_initial_pcp_dei_map_type_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_init_ttl_context_selection_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_pcp_dei_explicit_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_ive_untag_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_propag_profile_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_decrement_configuration_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_pipe_profile_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_initial_select_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_jer_ingress_nwk_qos_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_tc_nwk_qos_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_nwk_qos_type_select_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_fwd_layer_type_select_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_model_mapping_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_map_type_select_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_protocol_profile_to_internal_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_mapping_initial_network_qos_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_pcp_dei_dp_mapping_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_profile_to_internal_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_fwd_header_remark_enabler_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_mapping_packet_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_network_qos_mapping_next_layer_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_remark_profile_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_pipe_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_ttl_model_configuration_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_ingress_inheritance_resolution_mpls_type_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_policer_offset_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_jr1_fhei_model_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_enc_qos_var_type_mapping_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_fwd_plus_1_remakr_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_profile_to_fwd_plus_1_internal_map_id_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_qos_hl_pp_qos_tables_definition_qos_egress_cos_profile_table_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
