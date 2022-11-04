
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_header_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_HEADER;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_HEADER, is_valid, maturity_level, "ITPP_HEADER", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            db_field.nof_instances = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_HEADER_0r, LAST_RCVD_HDR_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_HEADER_1r, LAST_RCVD_HDR_1f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_HEADER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_header_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_HEADER;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_HEADER, is_valid, maturity_level, "ITPPD_HEADER", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            db_field.nof_instances = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_HEADER_0r, LAST_RCVD_HDR_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_HEADER_1r, LAST_RCVD_HDR_1f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_HEADER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_attributes_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_ATTRIBUTES;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_ATTRIBUTES, is_valid, maturity_level, "ITPP_ATTRIBUTES", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 12);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 14;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CNI, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACTION_TYPE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PD_CUD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PD_CUD_TYPE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_DSP, DBAL_FIELD_TYPE_DEF_DSP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_DEVICE_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_FABRIC, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DRAM, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QNUM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAC_TS, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PACKET_SIZEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_DPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CNI;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_CNIf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACTION_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_ACTION_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PD_CUD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PD_CUDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PD_CUD_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PD_CUD_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PP_DSP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PP_DSPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_DEVICE_ID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_DEST_DEVICE_IDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_FABRIC;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_IS_FABRICf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_DRAM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_IS_IS_DRAMf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_QNUM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_QNUMf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAC_TS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_MAC_TSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_ATTRIBUTES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_attributes_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_ATTRIBUTES;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_ATTRIBUTES, is_valid, maturity_level, "ITPPD_ATTRIBUTES", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 12);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 14;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CNI, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACTION_TYPE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PD_CUD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PD_CUD_TYPE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_DSP, DBAL_FIELD_TYPE_DEF_DSP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_DEVICE_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_FABRIC, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DRAM, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_QNUM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAC_TS, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PACKET_SIZEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_DPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CNI;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_CNIf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACTION_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_ACTION_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PD_CUD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PD_CUDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PD_CUD_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PD_CUD_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PP_DSP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_PP_DSPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_DEVICE_ID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_DEST_DEVICE_IDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_FABRIC;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_IS_FABRICf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_DRAM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_IS_IS_DRAMf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_QNUM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_QNUMf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAC_TS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_RECEIVED_ATTRIBUTESr, LAST_RVCD_MAC_TSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_ATTRIBUTES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_generated_values_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_GENERATED_VALUES;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_GENERATED_VALUES, is_valid, maturity_level, "ITPP_GENERATED_VALUES", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEM_SOFT_ERR, DBAL_FIELD_TYPE_DEF_MEM_SOFT_ERR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BYTES_TO_ADD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BYTES_TO_REMOVE, DBAL_FIELD_TYPE_DEF_BYTES_TO_REMOVE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DISCARD, DBAL_FIELD_TYPE_DEF_DISCARD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEM_SOFT_ERR;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_MEM_SOFT_ERRf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BYTES_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_BYTES_TO_ADDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BYTES_TO_REMOVE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_BYTES_TO_REMOVEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DISCARD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_DISCARD_PKTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_GENERATED_VALUES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_generated_values_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_GENERATED_VALUES;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_GENERATED_VALUES, is_valid, maturity_level, "ITPPD_GENERATED_VALUES", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEM_SOFT_ERR, DBAL_FIELD_TYPE_DEF_MEM_SOFT_ERR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BYTES_TO_ADD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BYTES_TO_REMOVE, DBAL_FIELD_TYPE_DEF_BYTES_TO_REMOVE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DISCARD, DBAL_FIELD_TYPE_DEF_DISCARD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEM_SOFT_ERR;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_VALUESr, LAST_MEM_SOFT_ERRf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BYTES_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_VALUESr, LAST_BYTES_TO_ADDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BYTES_TO_REMOVE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_VALUESr, LAST_BYTES_TO_REMOVEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DISCARD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_VALUESr, LAST_DISCARD_PKTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_GENERATED_VALUES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_generated_headder_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_GENERATED_HEADDER;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_GENERATED_HEADDER, is_valid, maturity_level, "ITPP_GENERATED_HEADDER", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER_TO_ADD, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 256;
            db_field.nof_instances = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_HDRr, LAST_HEADER_TO_ADD_1f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_HEADER_TO_ADD_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_LAST_GENERATED_VALUESr, LAST_HEADER_TO_ADD_0f));
                    {
                        uint32 result_size__header_to_add = 0;
                        result_size__header_to_add += 256;
                        access_params->data_offset.formula_int = result_size__header_to_add;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_GENERATED_HEADDER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_generated_headder_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_GENERATED_HEADDER;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_GENERATED_HEADDER, is_valid, maturity_level, "ITPPD_GENERATED_HEADDER", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER_TO_ADD, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 256;
            db_field.nof_instances = 2;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_HDRr, LAST_HEADER_TO_ADD_1f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_LAST_GENERATED_VALUESr, LAST_HEADER_TO_ADD_0f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_GENERATED_HEADDER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_max_latency_lat_pkt_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_MAX_LATENCY_LAT_PKT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_MAX_LATENCY_LAT_PKT, is_valid, maturity_level, "ITPP_MAX_LATENCY_LAT_PKT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LATENCY_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CURRENT_TS_COUNT_VAL, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_DEV_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_PORT_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRF_CLS_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LATENCY_VALUE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCY_LAT_PKTr, MAX_LATENCY_VALUEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CURRENT_TS_COUNT_VAL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCY_LAT_PKTr, CURRENT_TS_COUNT_VALf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_DEV_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCY_LAT_PKTr, DEST_DEV_LAT_PKTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_PORT_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCY_LAT_PKTr, DEST_PORT_LAT_PKTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TRF_CLS_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCY_LAT_PKTr, TRF_CLS_LAT_PKTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_MAX_LATENCY_LAT_PKT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_max_latency_lat_pkt_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_MAX_LATENCY_LAT_PKT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_MAX_LATENCY_LAT_PKT, is_valid, maturity_level, "ITPPD_MAX_LATENCY_LAT_PKT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LATENCY_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CURRENT_TS_COUNT_VAL, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_DEV_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEST_PORT_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRF_CLS_LAT_PKT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LATENCY_VALUE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCY_LAT_PKTr, MAX_LATENCY_VALUEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CURRENT_TS_COUNT_VAL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCY_LAT_PKTr, CURRENT_TS_COUNT_VALf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_DEV_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCY_LAT_PKTr, DEST_DEV_LAT_PKTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEST_PORT_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCY_LAT_PKTr, DEST_PORT_LAT_PKTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TRF_CLS_LAT_PKT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCY_LAT_PKTr, TRF_CLS_LAT_PKTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_MAX_LATENCY_LAT_PKT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_deleted_lat_pkts_cnt_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_DELETED_LAT_PKTS_CNT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_DELETED_LAT_PKTS_CNT, is_valid, maturity_level, "ITPP_DELETED_LAT_PKTS_CNT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DELETED_LAT_PKTS_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DELETED_LAT_PKTS_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DELETED_LAT_PKTS_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_DELETED_LAT_PKTS_CNTr, DELETED_LAT_PKTS_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DELETED_LAT_PKTS_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_DELETED_LAT_PKTS_CNTr, DELETED_LAT_PKTS_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_DELETED_LAT_PKTS_CNT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_deleted_lat_pkts_cnt_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_DELETED_LAT_PKTS_CNT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_DELETED_LAT_PKTS_CNT, is_valid, maturity_level, "ITPPD_DELETED_LAT_PKTS_CNT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DELETED_LAT_PKTS_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DELETED_LAT_PKTS_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DELETED_LAT_PKTS_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_DELETED_LAT_PKTS_CNTr, DELETED_LAT_PKTS_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DELETED_LAT_PKTS_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_DELETED_LAT_PKTS_CNTr, DELETED_LAT_PKTS_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_DELETED_LAT_PKTS_CNT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_big_lat_cni_cntr_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_BIG_LAT_CNI_CNTR;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_BIG_LAT_CNI_CNTR, is_valid, maturity_level, "ITPP_BIG_LAT_CNI_CNTR", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BIG_LAT_CNI_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BIG_LAT_CNI_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BIG_LAT_CNI_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_BIG_LAT_CNI_CNTRr, BIG_LAT_CNI_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BIG_LAT_CNI_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_BIG_LAT_CNI_CNTRr, BIG_LAT_CNI_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_BIG_LAT_CNI_CNTR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_big_lat_cni_cntr_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_BIG_LAT_CNI_CNTR;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_BIG_LAT_CNI_CNTR, is_valid, maturity_level, "ITPPD_BIG_LAT_CNI_CNTR", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BIG_LAT_CNI_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BIG_LAT_CNI_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BIG_LAT_CNI_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_BIG_LAT_CNI_CNTRr, BIG_LAT_CNI_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BIG_LAT_CNI_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_BIG_LAT_CNI_CNTRr, BIG_LAT_CNI_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_BIG_LAT_CNI_CNTR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_incoming_packet_cntr_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_INCOMING_PACKET_CNTR;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_INCOMING_PACKET_CNTR, is_valid, maturity_level, "ITPP_INCOMING_PACKET_CNTR", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INCOMING_PACKET_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INCOMING_PACKET_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INCOMING_PACKET_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_INCOMING_PACKET_CNTRr, INCOMING_PACKET_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INCOMING_PACKET_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_INCOMING_PACKET_CNTRr, INCOMING_PACKET_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_INCOMING_PACKET_CNTR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_incoming_packet_cntr_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_INCOMING_PACKET_CNTR;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_INCOMING_PACKET_CNTR, is_valid, maturity_level, "ITPPD_INCOMING_PACKET_CNTR", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INCOMING_PACKET_CNT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 31;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INCOMING_PACKET_CNT_OVF, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INCOMING_PACKET_CNT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_INCOMING_PACKET_CNTRr, INCOMING_PACKET_CNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INCOMING_PACKET_CNT_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_INCOMING_PACKET_CNTRr, INCOMING_PACKET_CNT_OVFf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_INCOMING_PACKET_CNTR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_max_latency_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPP_MAX_LATENCY;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPP_MAX_LATENCY, is_valid, maturity_level, "ITPP_MAX_LATENCY", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_POINTER, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_PKT_WAS_DROP, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_CURRENT_TS_COUNT, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_TRF_CLS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_PORT_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_DEST_DEV, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_VALUE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_VALUEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_POINTER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_POINTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_PKT_WAS_DROP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_PKT_WAS_DROPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_CURRENT_TS_COUNT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_CURRENT_TS_COUNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_TRF_CLS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_TRF_CLSf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_PORT_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_PORT_IDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_DEST_DEV;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_MAX_LATENCYm, LAT_DEST_DEVf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPP_MAX_LATENCY, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_max_latency_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ITPPD_MAX_LATENCY;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ITPPD_MAX_LATENCY, is_valid, maturity_level, "ITPPD_MAX_LATENCY", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_POINTER, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 20;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_PKT_WAS_DROP, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_CURRENT_TS_COUNT, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_TRF_CLS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_PORT_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAT_DEST_DEV, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_VALUE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_VALUEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_POINTER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_POINTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_PKT_WAS_DROP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_PKT_WAS_DROPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_CURRENT_TS_COUNT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_CURRENT_TS_COUNTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_TRF_CLS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_TRF_CLSf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_PORT_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_PORT_IDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAT_DEST_DEV;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_MAX_LATENCYm, LAT_DEST_DEVf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ITPPD_MAX_LATENCY, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_lbp_debug_signals_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_LBP_DEBUG_SIGNALS;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_LBP_DEBUG_SIGNALS, is_valid, maturity_level, "LBP_DEBUG_SIGNALS", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3 + 11);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SIG_BYTES_TO_ADD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SIG_BYTES_TO_REMOVE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADER_DATA, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 512;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TM_CMD, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.nof_instances = 11;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SIG_BYTES_TO_REMOVE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_DEBUG_REGISTER_1r, IHB_BYTES_TO_STRIPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SIG_BYTES_TO_ADD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_DEBUG_REGISTER_1r, IHB_BYTES_TO_PREPENDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADER_DATA;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_DEBUG_REGISTER_2r, IHB_HEADER_DATAf));
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_2r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_1_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 9;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_2r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_1_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TM_CMD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 10;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMD_HALF_2r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_1_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    {
                        int field_instance = 0;
                        for (field_instance = 0; field_instance < 11; field_instance++)
                        {
                            access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                            dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                            access_counter++;
                            access_params->access_field_id = DBAL_FIELD_TM_CMD;
                            access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                            access_params->inst_idx = field_instance;
                            {
                                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_IHB_TM_CMDr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                                access_params->data_offset.formula_cb = lbp_debug_signals_tm_cmd_dataoffset_0_cb;
                            }
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_LBP_DEBUG_SIGNALS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_ipmf3_last_fes_debug_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_IPMF3_LAST_FES_DEBUG;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_IPMF3_LAST_FES_DEBUG, is_valid, maturity_level, "IPMF3_LAST_FES_DEBUG", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYSTEM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTEXT, DBAL_FIELD_TYPE_DEF_ACL_CONTEXT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPMF3_ACTION, DBAL_FIELD_TYPE_DEF_IPMF3_ACTION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FES_ACTION_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FES_DATA_IN, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CONTEXT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_DBG_LAST_FESr, LAST_CONTEXTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPMF3_ACTION;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_DBG_LAST_FESr, OUT_ACTION_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FES_ACTION_VALUE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_DBG_LAST_FESr, OUT_ACTION_VALUEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FES_DATA_IN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_DBG_LAST_FESr, IN_KEYf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_IPMF3_LAST_FES_DEBUG, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_debug_flp_datapath_traps_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_DEBUG_FLP_DATAPATH_TRAPS;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_DEBUG_FLP_DATAPATH_TRAPS, is_valid, maturity_level, "DEBUG_FLP_DATAPATH_TRAPS", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_DIAGNOSTICS;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 37);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESERVED_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHERNET_SA_MC_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHERNET_SA_EQUAL_DA_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_VERSION_ERROR_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_CHECKSUM_ERROR_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_TOTAL_LENGTH_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_TTL0_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_HAS_OPTIONS_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_TTL1_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_SIP_EQUAL_DIP_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_DIP_ZERO_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_SIP_IS_MC_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_VERSION_ERROR_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_HOP_COUNT0_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_HOP_COUNT1_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_UNSPECIFIED_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_LOOPBACK_ADDRESS_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_MULTICAST_SOURCE_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_NEXT_HEADER_NULL_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_UNSPECIFIED_SOURCE_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_LINK_LOCAL_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_SITE_LOCAL_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_LINK_LOCAL_SOURCE_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_SITE_LOCAL_SOURCE_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_IPV4_COMPATIBLE_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_IPV4_MAPPED_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV6_MULTICAST_DESTINATION_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_TTL0_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_TTL1_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_SN_FLAGS_ZERO_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_SN_ZERO_FLAGS_SET_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_SYN_FUN_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_EQUAL_PORTS_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_FRAGMENT_INCOMPLETE_HEADER_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCP_FRAGMENT_OFFSET_LT8_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UDP_EQUAL_PORTS_TRAP_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FCOE_FABRIC_PROVIDED_SECURITY_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESERVED_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ETHERNET_SA_MC_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ETHERNET_SA_EQUAL_DA_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_VERSION_ERROR_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_CHECKSUM_ERROR_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_TOTAL_LENGTH_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_TTL0_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_HAS_OPTIONS_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_TTL1_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_SIP_EQUAL_DIP_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_DIP_ZERO_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_SIP_IS_MC_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_VERSION_ERROR_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_HOP_COUNT0_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_HOP_COUNT1_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_UNSPECIFIED_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_LOOPBACK_ADDRESS_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_MULTICAST_SOURCE_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_NEXT_HEADER_NULL_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_UNSPECIFIED_SOURCE_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_LINK_LOCAL_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_SITE_LOCAL_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_LINK_LOCAL_SOURCE_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_SITE_LOCAL_SOURCE_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_IPV4_COMPATIBLE_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_IPV4_MAPPED_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV6_MULTICAST_DESTINATION_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_TTL0_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_TTL1_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_SN_FLAGS_ZERO_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_SN_ZERO_FLAGS_SET_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_SYN_FUN_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_EQUAL_PORTS_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        uint32 result_size__tcp_syn_fun_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        result_size__tcp_syn_fun_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable + result_size__tcp_syn_fun_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_FRAGMENT_INCOMPLETE_HEADER_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        uint32 result_size__tcp_syn_fun_trap_enable = 0;
                        uint32 result_size__tcp_equal_ports_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        result_size__tcp_syn_fun_trap_enable += 1;
                        result_size__tcp_equal_ports_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable + result_size__tcp_syn_fun_trap_enable + result_size__tcp_equal_ports_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCP_FRAGMENT_OFFSET_LT8_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        uint32 result_size__tcp_syn_fun_trap_enable = 0;
                        uint32 result_size__tcp_equal_ports_trap_enable = 0;
                        uint32 result_size__tcp_fragment_incomplete_header_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        result_size__tcp_syn_fun_trap_enable += 1;
                        result_size__tcp_equal_ports_trap_enable += 1;
                        result_size__tcp_fragment_incomplete_header_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable + result_size__tcp_syn_fun_trap_enable + result_size__tcp_equal_ports_trap_enable + result_size__tcp_fragment_incomplete_header_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_UDP_EQUAL_PORTS_TRAP_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        uint32 result_size__tcp_syn_fun_trap_enable = 0;
                        uint32 result_size__tcp_equal_ports_trap_enable = 0;
                        uint32 result_size__tcp_fragment_incomplete_header_trap_enable = 0;
                        uint32 result_size__tcp_fragment_offset_lt8_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        result_size__tcp_syn_fun_trap_enable += 1;
                        result_size__tcp_equal_ports_trap_enable += 1;
                        result_size__tcp_fragment_incomplete_header_trap_enable += 1;
                        result_size__tcp_fragment_offset_lt8_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable + result_size__tcp_syn_fun_trap_enable + result_size__tcp_equal_ports_trap_enable + result_size__tcp_fragment_incomplete_header_trap_enable + result_size__tcp_fragment_offset_lt8_trap_enable;
                    }
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FCOE_FABRIC_PROVIDED_SECURITY_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_DBG_FLP_DATA_PATH_TRAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        uint32 result_size__reserved_trap_enable = 0;
                        uint32 result_size__ethernet_sa_mc_trap_enable = 0;
                        uint32 result_size__ethernet_sa_equal_da_trap_enable = 0;
                        uint32 result_size__ipv4_version_error_trap_enable = 0;
                        uint32 result_size__ipv4_checksum_error_trap_enable = 0;
                        uint32 result_size__ipv4_total_length_trap_enable = 0;
                        uint32 result_size__ipv4_ttl0_trap_enable = 0;
                        uint32 result_size__ipv4_has_options_trap_enable = 0;
                        uint32 result_size__ipv4_ttl1_trap_enable = 0;
                        uint32 result_size__ipv4_sip_equal_dip_trap_enable = 0;
                        uint32 result_size__ipv4_dip_zero_trap_enable = 0;
                        uint32 result_size__ipv4_sip_is_mc_trap_enable = 0;
                        uint32 result_size__ipv6_version_error_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count0_trap_enable = 0;
                        uint32 result_size__ipv6_hop_count1_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_destination_trap_enable = 0;
                        uint32 result_size__ipv6_loopback_address_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_source_trap_enable = 0;
                        uint32 result_size__ipv6_next_header_null_trap_enable = 0;
                        uint32 result_size__ipv6_unspecified_source_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_destination_trap_enable = 0;
                        uint32 result_size__ipv6_link_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_site_local_source_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_compatible_destination_trap_enable = 0;
                        uint32 result_size__ipv6_ipv4_mapped_destination_trap_enable = 0;
                        uint32 result_size__ipv6_multicast_destination_trap_enable = 0;
                        uint32 result_size__mpls_ttl0_trap_enable = 0;
                        uint32 result_size__mpls_ttl1_trap_enable = 0;
                        uint32 result_size__tcp_sn_flags_zero_trap_enable = 0;
                        uint32 result_size__tcp_sn_zero_flags_set_trap_enable = 0;
                        uint32 result_size__tcp_syn_fun_trap_enable = 0;
                        uint32 result_size__tcp_equal_ports_trap_enable = 0;
                        uint32 result_size__tcp_fragment_incomplete_header_trap_enable = 0;
                        uint32 result_size__tcp_fragment_offset_lt8_trap_enable = 0;
                        uint32 result_size__udp_equal_ports_trap_enable = 0;
                        result_size__reserved_trap_enable += 1;
                        result_size__ethernet_sa_mc_trap_enable += 1;
                        result_size__ethernet_sa_equal_da_trap_enable += 1;
                        result_size__ipv4_version_error_trap_enable += 1;
                        result_size__ipv4_checksum_error_trap_enable += 1;
                        result_size__ipv4_total_length_trap_enable += 1;
                        result_size__ipv4_ttl0_trap_enable += 1;
                        result_size__ipv4_has_options_trap_enable += 1;
                        result_size__ipv4_ttl1_trap_enable += 1;
                        result_size__ipv4_sip_equal_dip_trap_enable += 1;
                        result_size__ipv4_dip_zero_trap_enable += 1;
                        result_size__ipv4_sip_is_mc_trap_enable += 1;
                        result_size__ipv6_version_error_trap_enable += 1;
                        result_size__ipv6_hop_count0_trap_enable += 1;
                        result_size__ipv6_hop_count1_trap_enable += 1;
                        result_size__ipv6_unspecified_destination_trap_enable += 1;
                        result_size__ipv6_loopback_address_trap_enable += 1;
                        result_size__ipv6_multicast_source_trap_enable += 1;
                        result_size__ipv6_next_header_null_trap_enable += 1;
                        result_size__ipv6_unspecified_source_trap_enable += 1;
                        result_size__ipv6_link_local_destination_trap_enable += 1;
                        result_size__ipv6_site_local_destination_trap_enable += 1;
                        result_size__ipv6_link_local_source_trap_enable += 1;
                        result_size__ipv6_site_local_source_trap_enable += 1;
                        result_size__ipv6_ipv4_compatible_destination_trap_enable += 1;
                        result_size__ipv6_ipv4_mapped_destination_trap_enable += 1;
                        result_size__ipv6_multicast_destination_trap_enable += 1;
                        result_size__mpls_ttl0_trap_enable += 1;
                        result_size__mpls_ttl1_trap_enable += 1;
                        result_size__tcp_sn_flags_zero_trap_enable += 1;
                        result_size__tcp_sn_zero_flags_set_trap_enable += 1;
                        result_size__tcp_syn_fun_trap_enable += 1;
                        result_size__tcp_equal_ports_trap_enable += 1;
                        result_size__tcp_fragment_incomplete_header_trap_enable += 1;
                        result_size__tcp_fragment_offset_lt8_trap_enable += 1;
                        result_size__udp_equal_ports_trap_enable += 1;
                        access_params->data_offset.formula_int = result_size__reserved_trap_enable + result_size__ethernet_sa_mc_trap_enable + result_size__ethernet_sa_equal_da_trap_enable + result_size__ipv4_version_error_trap_enable + result_size__ipv4_checksum_error_trap_enable + result_size__ipv4_total_length_trap_enable + result_size__ipv4_ttl0_trap_enable + result_size__ipv4_has_options_trap_enable + result_size__ipv4_ttl1_trap_enable + result_size__ipv4_sip_equal_dip_trap_enable + result_size__ipv4_dip_zero_trap_enable + result_size__ipv4_sip_is_mc_trap_enable + result_size__ipv6_version_error_trap_enable + result_size__ipv6_hop_count0_trap_enable + result_size__ipv6_hop_count1_trap_enable + result_size__ipv6_unspecified_destination_trap_enable + result_size__ipv6_loopback_address_trap_enable + result_size__ipv6_multicast_source_trap_enable + result_size__ipv6_next_header_null_trap_enable + result_size__ipv6_unspecified_source_trap_enable + result_size__ipv6_link_local_destination_trap_enable + result_size__ipv6_site_local_destination_trap_enable + result_size__ipv6_link_local_source_trap_enable + result_size__ipv6_site_local_source_trap_enable + result_size__ipv6_ipv4_compatible_destination_trap_enable + result_size__ipv6_ipv4_mapped_destination_trap_enable + result_size__ipv6_multicast_destination_trap_enable + result_size__mpls_ttl0_trap_enable + result_size__mpls_ttl1_trap_enable + result_size__tcp_sn_flags_zero_trap_enable + result_size__tcp_sn_zero_flags_set_trap_enable + result_size__tcp_syn_fun_trap_enable + result_size__tcp_equal_ports_trap_enable + result_size__tcp_fragment_incomplete_header_trap_enable + result_size__tcp_fragment_offset_lt8_trap_enable + result_size__udp_equal_ports_trap_enable;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_DEBUG_FLP_DATAPATH_TRAPS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_invalid_destination_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INVALID_DESTINATION;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INVALID_DESTINATION, is_valid, maturity_level, "INVALID_DESTINATION", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYSTEM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_DESTINATION_INVALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INVALID_DESTINATION, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 19;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INVALID_DESTINATION_SOURCE, DBAL_FIELD_TYPE_DEF_INVALID_DESTINATION_SOURCE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_DESTINATION_INVALID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_INVALID_DESTINATIONr, INVALID_DESTINATION_INDICATORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INVALID_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_INVALID_DESTINATIONr, INVALID_DESTINATION_DESTINATIONf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INVALID_DESTINATION_SOURCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_INVALID_DESTINATIONr, INVALID_DESTINATION_SOURCEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INVALID_DESTINATION, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_header_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_header_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_attributes_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_attributes_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_generated_values_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_generated_values_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_generated_headder_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_generated_headder_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_max_latency_lat_pkt_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_max_latency_lat_pkt_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_deleted_lat_pkts_cnt_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_deleted_lat_pkts_cnt_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_big_lat_cni_cntr_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_big_lat_cni_cntr_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_incoming_packet_cntr_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_incoming_packet_cntr_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itpp_max_latency_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_itppd_max_latency_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_lbp_debug_signals_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_ipmf3_last_fes_debug_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_debug_flp_datapath_traps_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_global_hl_pp_debug_ipp_invalid_destination_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
