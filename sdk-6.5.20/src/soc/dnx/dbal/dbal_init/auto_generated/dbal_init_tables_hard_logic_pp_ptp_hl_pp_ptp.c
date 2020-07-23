
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_ieee_1588_identification_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION , is_valid , "PTP_IEEE_1588_IDENTIFICATION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_TCAM_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_PTP;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 8 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "PTP_IEEE_1588_IDENTIFICATION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_0 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_1 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_2 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_3 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_4 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_5 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_6 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FORWARDING_TYPE_7 , DBAL_FIELD_TYPE_DEF_LAYER_TYPES , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 8);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PACKET_IS_IEEE_1588 , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NONE_UDP_ENCAPSULATION , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION, is_standard_1, is_compatible_with_all_images));
    }
    
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int access_counter = 0;
        table_db_access_params_struct_t * access_params;
        int map_idx = 0;
        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
        access_counter++;
        
        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
        
        
        access_params->tcam_access_type = DBAL_HL_TCAM_ACCESS_TYPE_KEY;
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
        
        {
            int regMemHwEntityId = INVALIDm;
            int fieldHwEntityId = INVALIDf;
            
            regMemHwEntityId = IPPB_IEEE_1588_IDENTIFICATION_CAMm;
            
            fieldHwEntityId = KEYf;
            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
        }
        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
        access_counter++;
        
        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
        
        
        access_params->tcam_access_type = DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK;
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
        
        {
            int regMemHwEntityId = INVALIDm;
            int fieldHwEntityId = INVALIDf;
            
            regMemHwEntityId = IPPB_IEEE_1588_IDENTIFICATION_CAMm;
            
            fieldHwEntityId = INVALIDf;
            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
            {
                access_params->data_offset.formula_int = 40; 
            }
        }
        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
        access_counter++;
        
        SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE , TRUE ););
        
        
        access_params->tcam_access_type = DBAL_HL_TCAM_ACCESS_TYPE_RESULT;
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
        
        {
            int regMemHwEntityId = INVALIDm;
            int fieldHwEntityId = INVALIDf;
            
            regMemHwEntityId = IPPB_IEEE_1588_IDENTIFICATION_CAMm;
            
            fieldHwEntityId = PROGRAMf;
            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PTP_IEEE_1588_IDENTIFICATION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_egress_tp_command_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PTP_1588_EGRESS_TP_COMMAND;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_PTP_1588_EGRESS_TP_COMMAND , is_valid , "PTP_1588_EGRESS_TP_COMMAND" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_PTP;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 1 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "PTP_1588_EGRESS_TP_COMMAND" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TP_COMMAND , DBAL_FIELD_TYPE_DEF_PTP_TP_COMMAND , dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TX_STAMP_CF , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RECORD_TS , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RX_STAMP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_PTP_1588_EGRESS_TP_COMMAND, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_TX_STAMP_CF + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_CFG_IEEE_1588_TX_STAMP_CFr;
                    
                    fieldHwEntityId = CFG_IEEE_1588_TX_STAMP_CFf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = ptp_1588_egress_tp_command_tx_stamp_cf_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_RECORD_TS + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_CFG_IEEE_1588_RECORD_TSr;
                    
                    fieldHwEntityId = CFG_IEEE_1588_RECORD_TSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = ptp_1588_egress_tp_command_tx_stamp_cf_dataoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_RX_STAMP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_CFG_IEEE_1588_RX_STAMPr;
                    
                    fieldHwEntityId = CFG_IEEE_1588_RX_STAMPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                    access_params->data_offset.formula_cb = ptp_1588_egress_tp_command_tx_stamp_cf_dataoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PTP_1588_EGRESS_TP_COMMAND, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_port_properties_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PTP_1588_PORT_PROPERTIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_PTP_1588_PORT_PROPERTIES , is_valid , "PTP_1588_PORT_PROPERTIES" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_PTP;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "PTP_1588_PORT_PROPERTIES" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT , DBAL_FIELD_TYPE_DEF_PP_PORT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_P2P_DELAY , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 26;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESET_TIME_STAMP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_USE_INGRESS_TIME_STAMP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTERNAL_BRCM_MAC , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IEEE_1588_MAC_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IEEE_1588_PROFILE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_INGRESS_P2P_DELAY + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPD_LBP_PP_PORT_INFOm;
                    
                    fieldHwEntityId = INGRESS_P2P_DELAYf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 2; 
                    }
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_RESET_TIME_STAMP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPD_LBP_PP_PORT_INFOm;
                    
                    fieldHwEntityId = RESET_TIME_STAMPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_USE_INGRESS_TIME_STAMP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPD_LBP_PP_PORT_INFOm;
                    
                    fieldHwEntityId = USE_INGRESS_TIME_STAMPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_EXTERNAL_BRCM_MAC + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPD_LBP_PP_PORT_INFOm;
                    
                    fieldHwEntityId = EXTERNAL_BRCM_MACf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IEEE_1588_MAC_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ETPPB_CFG_IEEE_1588_MAC_ENABLEr;
                        
                        fieldHwEntityId = CFG_IEEE_1588_MAC_ENABLEf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->data_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            
            {
                table_db_access_params_struct_t * access_params;
                
                {
                    int instance_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    
                    SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                    
                    
                    access_params->access_field_id = DBAL_FIELD_IEEE_1588_MAC_ENABLE + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDr;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = ETPPB_CFG_IEEE_1588_MAC_ENABLEr;
                        
                        fieldHwEntityId = CFG_IEEE_1588_MAC_ENABLE_Nf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                        access_params->array_offset.formula_cb = ptp_1588_port_properties_ieee_1588_mac_enable_arrayoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_port_properties_ieee_1588_mac_enable_dataoffset_0_cb;
                    }
                }
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
                
                
                access_params->access_field_id = DBAL_FIELD_IEEE_1588_PROFILE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = IPPB_PINFO_FLP_MEMm;
                    
                    fieldHwEntityId = IEEE_1588_PROFILEf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                    access_params->entry_offset.formula_cb = ptp_1588_port_properties_ingress_p2p_delay_entryoffset_0_cb;
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PTP_1588_PORT_PROPERTIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_global_config_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PTP_1588_GLOBAL_CONFIG;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_PTP_1588_GLOBAL_CONFIG , is_valid , "PTP_1588_GLOBAL_CONFIG" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_PTP;
    
    
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UDP_CHECKSUM_ENABLE , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TS_OFFSET , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_PTP_1588_GLOBAL_CONFIG, is_standard_1, is_compatible_with_all_images));
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
                
                
                access_params->access_field_id = DBAL_FIELD_UDP_CHECKSUM_ENABLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_CFG_TS_UDP_ENCAPr;
                    
                    fieldHwEntityId = CFG_TS_UDP_ENCAPf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
            
            {
                int instance_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, TRUE , FALSE ););
                
                
                access_params->access_field_id = DBAL_FIELD_UDP_CHECKSUM_ENABLE + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPC_DEVICE_CFG_TS_UDP_ENCAPr;
                    
                    fieldHwEntityId = DEVICE_CFG_TS_UDP_ENCAPf;
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
                
                
                access_params->access_field_id = DBAL_FIELD_TS_OFFSET + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                
                {
                    int regMemHwEntityId = INVALIDr;
                    int fieldHwEntityId = INVALIDf;
                    
                    regMemHwEntityId = ETPPB_CFG_IEEE_1588_TS_OFFSET_FIXr;
                    
                    fieldHwEntityId = CFG_IEEE_1588_TS_OFFSET_FIXf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDr ));
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PTP_1588_GLOBAL_CONFIG, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_action_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PTP_1588_ACTION;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_PTP_1588_ACTION , is_valid , "PTP_1588_ACTION" , DBAL_ACCESS_METHOD_HARD_LOGIC ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    
    
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_PTP;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 5 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "PTP_1588_ACTION" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_TWO_STEP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MESSAGE_TYPE , DBAL_FIELD_TYPE_DEF_PTP_1588_MSG_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADDRESS , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NONE_UDP_ENCAPSULATION , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_PROFILE , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
        dbal_db_init_table_and_db_fields_set_allocator_fields(unit, table_entry, 5);
    }
    
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3 + (((dnx_data_ptp.general.feature_get(unit, dnx_data_ptp_general_has_trap_code_offset))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UPDATE_TIME_STAMP_COMPENSATION , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TP_COMMAND , DBAL_FIELD_TYPE_DEF_PTP_TP_COMMAND , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACTION_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ptp.general.feature_get(unit, dnx_data_ptp_general_has_trap_code_offset) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CODE_INDEX , DBAL_FIELD_TYPE_DEF_UINT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ptp.general.feature_get(unit, dnx_data_ptp_general_has_trap_code_offset);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_PTP_1588_ACTION, is_standard_1, is_compatible_with_all_images));
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_UPDATE_TIME_STAMP_COMPENSATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = UPDATE_TIME_STAMPf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_UPDATE_TIME_STAMP_COMPENSATION + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = UPDATE_TIME_STAMPf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_1_cb;
                    }
                }
            }
        }
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_TP_COMMAND + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = UPDATE_COMMANDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_TP_COMMAND + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = UPDATE_COMMANDf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_1_cb;
                    }
                }
            }
        }
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ACTION_INDEX + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = ACTION_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_0_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_ACTION_INDEX + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = ACTION_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_1_cb;
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
                    
                    
                    access_params->access_field_id = DBAL_FIELD_CODE_INDEX + instance_index;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 , 0 , DBAL_VALUE_FIELD_ENCODE_NONE , 0 ));
                    
                    {
                        int regMemHwEntityId = INVALIDm;
                        int fieldHwEntityId = INVALIDf;
                        
                        regMemHwEntityId = IPPB_IEEE_1588_ACTIONm;
                        
                        fieldHwEntityId = CODE_INDEXf;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY , regMemHwEntityId , fieldHwEntityId , INVALIDm ));
                        access_params->entry_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ptp_1588_action_update_time_stamp_compensation_dataoffset_1_cb;
                    }
                }
            }
        }
        
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        
        map_idx++;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PTP_1588_ACTION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_ieee_1588_identification_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_egress_tp_command_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_port_properties_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_global_config_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_ptp_hl_pp_ptp_ptp_1588_action_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
