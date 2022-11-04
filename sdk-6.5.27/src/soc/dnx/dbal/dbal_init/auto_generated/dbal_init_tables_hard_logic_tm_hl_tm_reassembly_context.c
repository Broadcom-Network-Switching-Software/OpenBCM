
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_port_to_context_map_base_address_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_PORT_TO_CONTEXT_MAP_BASE_ADDRESS_TABLE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_PORT_TO_CONTEXT_MAP_BASE_ADDRESS_TABLE, is_valid, maturity_level, "INGR_REASSEMBLY_PORT_TO_CONTEXT_MAP_BASE_ADDRESS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_REASSEMBLY_INTERFACE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.interface_bits_get(unit);
            db_field.max_value = dnx_data_ingr_reassembly.dbal.interface_max_get(unit);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.context_map_base_address_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_INTERLEAVED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERLEAVED_INTERFACE_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.interleaved_interface_bits_get(unit);
            db_field.max_value = dnx_data_ingr_reassembly.dbal.interleaved_interface_max_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    if (DBAL_IS_JR2_A0)
    {
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
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_PORT_TO_BASE_ADDRESS_MAPm, CTXT_MAP_BASE_ADDRESSf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_INTERLEAVED;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_PORT_TO_BASE_ADDRESS_MAPm, NIF_PORT_IS_CHANNELIZEDf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERLEAVED_INTERFACE_ID;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_PORT_TO_BASE_ADDRESS_MAPm, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else
    {
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
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PORT_ATTRm, CTXT_MAP_BASE_ADDRESSf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_INTERLEAVED;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PORT_ATTRm, NIF_PORT_IS_CHANNELIZEDf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERLEAVED_INTERFACE_ID;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PORT_ATTRm, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_PORT_TO_CONTEXT_MAP_BASE_ADDRESS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_context_map_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, is_valid, maturity_level, "INGR_REASSEMBLY_CONTEXT_MAP_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.context_map_base_address_bits_get(unit)+1;
            db_field.max_value = dnx_data_ingr_reassembly.dbal.context_map_base_offest_ext_max_get(unit);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT, DBAL_FIELD_TYPE_DEF_REASSEMBLY_CONTEXT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_TERMINATION_CONTEXT, DBAL_FIELD_TYPE_DEF_PTC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
    {
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
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+3, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_EVENTOR_REASSEMBLY_CONTEXT_CONFIGURATIONr, EVENTOR_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+2, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OAMP_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OLP_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CTXT_MAPm, REASSEMBLY_CONTEXTf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+3, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_EVENTOR_REASSEMBLY_CONTEXT_CONFIGURATIONr, EVENTOR_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+2, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OAMP_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OLP_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CTXT_MAPm, PORT_TERMINATION_CONTEXTf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else
    {
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
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+4, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CRPS_REASSEMBLY_CONTEXT_CONFIGURATIONr, CRPS_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+3, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_EVENTOR_REASSEMBLY_CONTEXT_CONFIGURATIONr, EVENTOR_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+2, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OAMP_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OLP_REASSEMBLY_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CTXT_MAPm, REASSEMBLY_CONTEXTf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+4, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CRPS_REASSEMBLY_CONTEXT_CONFIGURATIONr, CRPS_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+3, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_EVENTOR_REASSEMBLY_CONTEXT_CONFIGURATIONr, EVENTOR_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+2, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OAMP_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, OLP_PORT_TERMINATION_CONTEXTf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PORT_TERMINATION_CONTEXT;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_ADDR, dnx_data_ingr_reassembly.dbal.context_map_base_address_max_get(unit)+1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CTXT_MAPm, PORT_TERMINATION_CONTEXTf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_interleaved_reassembly_context_bitmap_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_INTERLEAVED_REASSEMBLY_CONTEXT_BITMAP;
    int is_valid = dnx_data_ingr_reassembly.ilkn.feature_get(unit, dnx_data_ingr_reassembly_ilkn_is_supported);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_INTERLEAVED_REASSEMBLY_CONTEXT_BITMAP, is_valid, maturity_level, "INGR_REASSEMBLY_INTERLEAVED_REASSEMBLY_CONTEXT_BITMAP", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 3);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERLEAVED_INTERFACE_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.interleaved_interface_bits_get(unit);
            db_field.max_value = dnx_data_ingr_reassembly.dbal.interleaved_interface_max_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT, DBAL_FIELD_TYPE_DEF_REASSEMBLY_CONTEXT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACTIVE, DBAL_FIELD_TYPE_DEF_ACTIVE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
                access_params->access_field_id = DBAL_FIELD_ACTIVE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PORT_TO_CTXT_BIT_MAPm, CONTEXTS_BIT_MAPPINGf));
                    access_params->entry_offset.formula_cb = ingr_reassembly_interleaved_reassembly_context_bitmap_active_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = ingress_cong_override_pp_port_with_reassembly_context_is_overide_enable_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_INTERLEAVED_REASSEMBLY_CONTEXT_BITMAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_special_interface_base_address_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_SPECIAL_INTERFACE_BASE_ADDRESS;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_SPECIAL_INTERFACE_BASE_ADDRESS, is_valid, maturity_level, "INGR_REASSEMBLY_SPECIAL_INTERFACE_BASE_ADDRESS", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, DBAL_FIELD_TYPE_DEF_REASSEMBLY_SPECIAL_INTERFACE_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.min_value = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_reassembly.dbal.context_map_base_address_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    if (DBAL_IS_JR2_A0)
    {
        table_entry->core_mode = DBAL_CORE_MODE_SBC;
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
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_PORT_TO_BASE_ADDRESS_MAPm, CTXT_MAP_BASE_ADDRESSf));
                        access_params->entry_offset.formula_cb = ingr_reassembly_special_interface_base_address_reassembly_context_map_base_address_entryoffset_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else
    {
        table_entry->core_mode = DBAL_CORE_MODE_SBC;
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
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_CPU));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_CPU_BASE_ADDRf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_MIRROR_LOSSLESS));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_EGQ_PRI_0_BASE_ADDRf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_MIRROR_HIGH_PRIORTY));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_EGQ_PRI_1_BASE_ADDRf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_MIRROR_LOW_PRIORTY));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_EGQ_PRI_2_BASE_ADDRf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_RCY));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_EGQ_PRI_3_BASE_ADDRf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_SAT));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_INTERFACE_BASE_ADDR_CONFIGURATIONr, INTERFACE_SAT_BASE_ADDRf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_SPECIAL_INTERFACE_BASE_ADDRESS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_bad_reassembly_context_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_BAD_REASSEMBLY_CONTEXT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_BAD_REASSEMBLY_CONTEXT, is_valid, maturity_level, "INGR_REASSEMBLY_BAD_REASSEMBLY_CONTEXT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT, DBAL_FIELD_TYPE_DEF_REASSEMBLY_CONTEXT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
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
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_STATIC_CONFIGURATIONr, BAD_REASSEMBLY_CONTEXT_VALIDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_REASSEMBLY_CONTEXT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_STATIC_CONFIGURATIONr, BAD_REASSEMBLY_CONTEXTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_BAD_REASSEMBLY_CONTEXT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_ire_counter_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER, is_valid, maturity_level, "INGR_REASSEMBLY_IRE_COUNTER", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 14);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RCYH_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RCYH_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RCYL_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RCYL_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CPU_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CPU_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OLP_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OLP_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OAMP_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OAMP_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER, DBAL_FIELD_TYPE_DEF_ARRAY32, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 63;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER_OVF, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
                access_params->access_field_id = DBAL_FIELD_NIF_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PACKET_COUNTERr, NIF_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NIF_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_NIF_PACKET_COUNTERr, NIF_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_RCYH_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_RCYH_PACKET_COUNTERr, RCYH_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RCYH_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_RCYH_PACKET_COUNTERr, RCYH_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_RCYL_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_RCYL_PACKET_COUNTERr, RCYL_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RCYL_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_RCYL_PACKET_COUNTERr, RCYL_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_CPU_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CPU_PACKET_COUNTERr, CPU_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CPU_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_CPU_PACKET_COUNTERr, CPU_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_OLP_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_PACKET_COUNTERr, OLP_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OLP_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OLP_PACKET_COUNTERr, OLP_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_OAMP_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_PACKET_COUNTERr, OAMP_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OAMP_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_OAMP_PACKET_COUNTERr, OAMP_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
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
                access_params->access_field_id = DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_REGISTER_INTERFACE_PACKET_COUNTERr, REGISTER_INTERFACE_PACKET_COUNTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER_OVF;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_REGISTER_INTERFACE_PACKET_COUNTERr, REGISTER_INTERFACE_PACKET_COUNTERf));
                    {
                        access_params->data_offset.formula_int = 63;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_timeout_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_TIMEOUT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_TIMEOUT, is_valid, maturity_level, "INGR_REASSEMBLY_TIMEOUT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_TIMEOUT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
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
                access_params->access_field_id = DBAL_FIELD_REASSEMBLY_TIMEOUT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SPB_REASSEMBLY_TIMEOUTr, REASSEMBLY_TIMEOUTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_TIMEOUT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_to_in_cgm_port_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_TO_IN_CGM_PORT_MAPPING;
    int is_valid = dnx_data_ingr_reassembly.context.feature_get(unit, dnx_data_ingr_reassembly_context_reassembly_to_in_cgm_port_mapping);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_TO_IN_CGM_PORT_MAPPING, is_valid, maturity_level, "INGR_REASSEMBLY_TO_IN_CGM_PORT_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REASSEMBLY_CONTEXT, DBAL_FIELD_TYPE_DEF_REASSEMBLY_CONTEXT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_CGM_PORT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_port.general.nof_port_bits_in_pp_bus_get(unit);
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
                access_params->access_field_id = DBAL_FIELD_IN_CGM_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SPB_PP_PORT_OWm, PP_PORTf));
                    access_params->entry_offset.formula_cb = ingress_cong_override_pp_port_with_reassembly_context_is_overide_enable_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_TO_IN_CGM_PORT_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_power_down_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGR_REASSEMBLY_POWER_DOWN;
    int is_valid = dnx_data_ingr_reassembly.general.feature_get(unit, dnx_data_ingr_reassembly_general_power_down_supported);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INGR_REASSEMBLY_POWER_DOWN, is_valid, maturity_level, "INGR_REASSEMBLY_POWER_DOWN", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POWER_DOWN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
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
                access_params->access_field_id = DBAL_FIELD_POWER_DOWN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IRE_SPECIAL_CLOCK_CONTROLSr, CLOCK_PD_POWER_DOWNf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGR_REASSEMBLY_POWER_DOWN, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_port_to_context_map_base_address_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_context_map_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_interleaved_reassembly_context_bitmap_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_special_interface_base_address_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_bad_reassembly_context_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_ire_counter_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_timeout_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_to_in_cgm_port_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_ingr_reassembly_power_down_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
