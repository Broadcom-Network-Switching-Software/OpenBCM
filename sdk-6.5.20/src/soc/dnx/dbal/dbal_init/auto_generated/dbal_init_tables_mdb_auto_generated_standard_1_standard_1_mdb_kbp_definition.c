
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_multicast_tcam_forward_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD , is_valid , "KBP_IPV4_MULTICAST_TCAM_FORWARD" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_TCAM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV4_MULTICAST_TCAM_FORWARD" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_DIP , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_SIP , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_GLOB_IN_LIF , DBAL_FIELD_TYPE_DEF_KBP_GLOB_IN_LIF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 24;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 4);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EEI , DBAL_FIELD_TYPE_DEF_EEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 24;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_EEI_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_ID , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_CMD , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_CMD , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_STAT_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF , DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 22;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_OUTLIF_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[1]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[2]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[3]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_unicast_private_lpm_forward_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD , is_valid , "KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4 , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 4);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EEI , DBAL_FIELD_TYPE_DEF_EEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 24;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_EEI_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_ID , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_CMD , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_CMD , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_STAT_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF , DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 22;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_OUTLIF_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[1]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[2]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[3]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_unicast_private_lpm_rpf_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF , is_valid , "KBP_IPV4_UNICAST_PRIVATE_LPM_RPF" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV4_UNICAST_PRIVATE_LPM_RPF" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4 , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
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
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_multicast_tcam_forward_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD , is_valid , "KBP_IPV6_MULTICAST_TCAM_FORWARD" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_TCAM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 5 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV6_MULTICAST_TCAM_FORWARD" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_DIP , DBAL_FIELD_TYPE_DEF_IPV6 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 128;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_SIP , DBAL_FIELD_TYPE_DEF_IPV6 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 128;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_GLOBAL_IN_LIF_21_16 , DBAL_FIELD_TYPE_DEF_KBP_GLOBAL_IN_LIF_21_16 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOBAL_IN_LIF_15_00 , DBAL_FIELD_TYPE_DEF_GLOBAL_IN_LIF_15_00 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 4);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EEI , DBAL_FIELD_TYPE_DEF_EEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 24;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_EEI_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_ID , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_CMD , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_CMD , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_STAT_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF , DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 22;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_OUTLIF_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[1]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[2]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[3]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_unicast_private_lpm_forward_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD , is_valid , "KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6 , DBAL_FIELD_TYPE_DEF_IPV6 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 128;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 4);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EEI , DBAL_FIELD_TYPE_DEF_EEI , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 24;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_EEI_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_ID , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_STAT_OBJECT_CMD , DBAL_FIELD_TYPE_DEF_STAT_OBJECT_CMD , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_DEST_STAT_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF , DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 22;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_FORWARD_OUTLIF_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[1]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[2]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[3]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_unicast_private_lpm_rpf_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF , is_valid , "KBP_IPV6_UNICAST_PRIVATE_LPM_RPF" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_IPV6_UNICAST_PRIVATE_LPM_RPF" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6 , DBAL_FIELD_TYPE_DEF_IPV6 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 128;
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
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_nat_external_to_internal_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_NAT_EXTERNAL_TO_INTERNAL;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_NAT_EXTERNAL_TO_INTERNAL , is_valid , "KBP_NAT_EXTERNAL_TO_INTERNAL" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_NAT_EXTERNAL_TO_INTERNAL" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IP_PROTOCOL , DBAL_FIELD_TYPE_DEF_IP_PROTOCOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_DIP , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L4_DST_PORT , DBAL_FIELD_TYPE_DEF_L4_DST_PORT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4 , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L4_PORT , DBAL_FIELD_TYPE_DEF_L4_PORT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_NAT_EXTERNAL_TO_INTERNAL, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_NAT_EXTERNAL_TO_INTERNAL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_nat_internal_to_external_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_NAT_INTERNAL_TO_EXTERNAL;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_NAT_INTERNAL_TO_EXTERNAL , is_valid , "KBP_NAT_INTERNAL_TO_EXTERNAL" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 4 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_NAT_INTERNAL_TO_EXTERNAL" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IP_PROTOCOL , DBAL_FIELD_TYPE_DEF_IP_PROTOCOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VRF , DBAL_FIELD_TYPE_DEF_VRF , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_SIP , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L4_SRC_PORT , DBAL_FIELD_TYPE_DEF_L4_SRC_PORT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
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
        
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4 , DBAL_FIELD_TYPE_DEF_IPV4 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L4_PORT , DBAL_FIELD_TYPE_DEF_L4_PORT , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_NAT_INTERNAL_TO_EXTERNAL, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_NAT_INTERNAL_TO_EXTERNAL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_pcc_state_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KBP_PCC_STATE_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KBP_PCC_STATE_TABLE , is_valid , "KBP_PCC_STATE_TABLE" , DBAL_ACCESS_METHOD_KBP ));
    
    
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    
    
    
    
    table_entry->table_type = DBAL_TABLE_TYPE_LPM;
    
    
    table_entry->nof_labels = 3;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_L2;
    table_entry->table_labels[1] = DBAL_LABEL_L3;
    table_entry->table_labels[2] = DBAL_LABEL_MPLS;
    
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KBP_PCC_STATE_TABLE" , EMPTY, EMPTY);
        
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_VIP_ID , DBAL_FIELD_TYPE_DEF_KBP_VIP_ID , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SIGNATURE , DBAL_FIELD_TYPE_DEF_SIGNATURE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
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
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        
        
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE , DBAL_FIELD_TYPE_DEF_RESULT_TYPE , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_1 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_1 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_ALIGNMENT_PADDING_2 , DBAL_FIELD_TYPE_DEF_KBP_ALIGNMENT_PADDING_2 , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA , DBAL_FIELD_TYPE_DEF_KBP_FORWARD_PMF_GENERIC_DATA , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_DROP , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DEFAULT , DBAL_FIELD_TYPE_DEF_BOOL , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION , DBAL_FIELD_TYPE_DEF_DESTINATION , dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 21;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "KBP_DEST_W_DEFAULT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter++;
    }
    
    
    
    
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    
    {
        
        uint8 is_standard_1 = TRUE;
        
        uint8 is_compatible_with_all_images = FALSE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KBP_PCC_STATE_TABLE, is_standard_1, is_compatible_with_all_images));
    }
    
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    
    
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_result_type_physical_values_by_reference_field(unit, &(table_entry->multi_res_info[0]), DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS));
    SHR_IF_ERR_EXIT(dbal_db_init_table_set_mdb_info(unit, cur_table_param, "KBP" , 0 , 0 , DBAL_STAGE_NONE ));
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KBP_PCC_STATE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_multicast_tcam_forward_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_unicast_private_lpm_forward_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv4_unicast_private_lpm_rpf_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_multicast_tcam_forward_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_unicast_private_lpm_forward_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_ipv6_unicast_private_lpm_rpf_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_nat_external_to_internal_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_nat_internal_to_external_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_mdb_auto_generated_standard_1_standard_1_mdb_kbp_definition_kbp_pcc_state_table_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
