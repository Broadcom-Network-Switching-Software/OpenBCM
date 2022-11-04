
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_clk_time_ratio_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_CLK_TIME_RATIO;
    int is_valid = dnx_data_instru.synced_counters.period_adjusted_to_ns_through_time_ratio_get(unit);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_CLK_TIME_RATIO, is_valid, maturity_level, "INST_SYNCED_COUNTERS_CLK_TIME_RATIO", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_INSTRU;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_CLOCK_CYCLES, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_instru.synced_counters.period_adjusting_clk_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_NANOSECONDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_instru.synced_counters.period_adjusting_ns_bits_get(unit);
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
                access_params->access_field_id = DBAL_FIELD_NOF_CLOCK_CYCLES;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_TIME_RATIOr, TIME_RATIO_NOF_CLKf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NOF_NANOSECONDS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_TIME_RATIOr, TIME_RATIO_NOF_NSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_CLK_TIME_RATIO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_CTRL, is_valid, maturity_level, "INST_SYNCED_COUNTERS_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_INSTRU;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SOURCE_TYPE, DBAL_FIELD_TYPE_DEF_SOURCE_TYPE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_START_TIME, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERVAL_PERIOD, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_instru.synced_counters.interval_period_size_hw_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_INTERVALS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_CONTINUOUS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_DONE_STATE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDU_INSTRUMENTATION_CTRLr, CDU_INST_START_TIMEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_TIMEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDB_INSTRUMENTATION_CTRLr, CDB_INST_START_TIMEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_TIMEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_TIMEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_START_TIME;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_NIF_INSTRUMENTATION_CTRLr, NIF_INST_START_TIMEf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDU_INSTRUMENTATION_CTRLr, CDU_INST_INTERVAL_PERIODf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_INTERVAL_PERIODf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDB_INSTRUMENTATION_CTRLr, CDB_INST_INTERVAL_PERIODf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_INTERVAL_PERIODf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_INTERVAL_PERIODf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INTERVAL_PERIOD;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_NIF_INSTRUMENTATION_CTRLr, NIF_INST_INTERVAL_PERIODf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDU_INSTRUMENTATION_CTRLr, CDU_INST_NOF_INTERVALSf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_NOF_INTERVALSf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDB_INSTRUMENTATION_CTRLr, CDB_INST_NOF_INTERVALSf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_NOF_INTERVALSf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_NOF_INTERVALSf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NOF_INTERVALS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_NIF_INSTRUMENTATION_CTRLr, NIF_INST_NOF_INTERVALSf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDU_INSTRUMENTATION_CTRLr, CDU_INST_OVERRIDE_MODEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_OVERRIDE_MODEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDB_INSTRUMENTATION_CTRLr, CDB_INST_OVERRIDE_MODEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_OVERRIDE_MODEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_OVERRIDE_MODEf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_NIF_INSTRUMENTATION_CTRLr, NIF_INST_OVERRIDE_MODEf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDU_INSTRUMENTATION_CTRLr, CDU_INST_START_AGAINf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_AGAINf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CDB_INSTRUMENTATION_CTRLr, CDB_INST_START_AGAINf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_AGAINf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_ICGM));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_CGM_INST_CTRLr, CGM_INST_START_AGAINf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLEAR_DONE_STATE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SOURCE_TYPE, 0, (uint32) DBAL_ENUM_FVAL_SOURCE_TYPE_NIF));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_NIF_INSTRUMENTATION_CTRLr, NIF_INST_START_AGAINf));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
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
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_TRIGGER_CTRL, is_valid, maturity_level, "INST_SYNCED_TRIGGER_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_INSTRU;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CGM_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_CGM_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_BROAD_VIEW_CTRLr, CGM_INSTRUMENTATION_ENABLEf));
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CDU_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_BROAD_VIEW_CTRLr, CDU_INSTRUMENTATION_ENABLEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CDU_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_BROAD_VIEW_CTRLr, CDB_INSTRUMENTATION_ENABLEf));
                    }
                }
            }
        }
        if (DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CDU_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_BROAD_VIEW_CTRLr, NIF_INSTRUMENTATION_ENABLEf));
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
                access_params->access_field_id = DBAL_FIELD_CRPS_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_BROAD_VIEW_CTRLr, CRPS_INSTRUMENTATION_ENABLEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
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
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM, is_valid, maturity_level, "INST_SYNCED_COUNTERS_SETTING_ICGM", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_INSTRU;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_INIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_CONTINUOUS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_INIT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_SETTINGSr, INSTRUMENTATION_INITf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNC_COUNTERS_NOF_INTERVALS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_SETTINGSr, INSTRUMENTATION_INTERVAL_NUMf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_CONTINUOUS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_SETTINGSr, INSTRUMENTATION_OVERRIDE_MODEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_SETTING_ICGM, table_info));
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
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN, is_valid, maturity_level, "INST_SYNCED_COUNTERS_LAST_COUNT_ICGN", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_WRITE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_WRITES, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
                access_params->access_field_id = DBAL_FIELD_LAST_WRITE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_STATUSr, INSTRUMENTATION_LAST_WRITTEN_ADDRf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NOF_WRITES;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_STATUSr, INSTRUMENTATION_WRITE_CTRf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_LAST_COUNT_ICGN, table_info));
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
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM, is_valid, maturity_level, "INST_SYNCED_COUNTERS_RECORDS_ICGM", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTERS_RECORD_INDEX, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 21 + (((dnx_data_instru.synced_counters.counters_info_get(unit,21,bcmInstruSyncedCountersTypeIcgm)->valid)>0)?1:0) + (((dnx_data_instru.synced_counters.counters_info_get(unit,22,bcmInstruSyncedCountersTypeIcgm)->valid)>0)?1:0) + (((dnx_data_instru.synced_counters.counters_info_get(unit,23,bcmInstruSyncedCountersTypeIcgm)->valid)>0)?1:0) + (((dnx_data_instru.synced_counters.counters_info_get(unit,24,bcmInstruSyncedCountersTypeIcgm)->valid)>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_BUFFERS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_PDBS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_DRAM_BDBS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_SHARED_WORDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_BUFFERS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_PDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_HEADROOM_WORDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_WORDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_WORDS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_BUFFERS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_0_BUFFERS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_1_BUFFERS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_PDBS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_PDBS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_PDBS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_PACKETS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_BYTES, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_REJECT_PACKETS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_REJECT_BYTES, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_PACKETS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_BYTES, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_instru.synced_counters.counters_info_get(unit,21,bcmInstruSyncedCountersTypeIcgm)->valid != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DRAM_ENQUEUE_PACKETS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_instru.synced_counters.counters_info_get(unit,21,bcmInstruSyncedCountersTypeIcgm)->valid;
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_instru.synced_counters.counters_info_get(unit,22,bcmInstruSyncedCountersTypeIcgm)->valid != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DRAM_ENQUEUE_BYTES, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_instru.synced_counters.counters_info_get(unit,22,bcmInstruSyncedCountersTypeIcgm)->valid;
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_instru.synced_counters.counters_info_get(unit,23,bcmInstruSyncedCountersTypeIcgm)->valid != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DRAM_DEQUEUE_PACKETS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_instru.synced_counters.counters_info_get(unit,23,bcmInstruSyncedCountersTypeIcgm)->valid;
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_instru.synced_counters.counters_info_get(unit,24,bcmInstruSyncedCountersTypeIcgm)->valid != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCED_COUNTERS_DRAM_DEQUEUE_BYTES, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_instru.synced_counters.counters_info_get(unit,24,bcmInstruSyncedCountersTypeIcgm)->valid;
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
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_BUFFERS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, SRAM_BUFFERS_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_OCB_PDBS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, SRAM_PDBS_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_DRAM_BDBS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, DRAM_BDBS_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_SHARED_WORDS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, WORDS_MIN_FREE_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_BUFFERS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, SRAM_BUFFERS_MIN_FREE_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VOQ_OCBS_SHARED_PDS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_0m, SRAM_PDS_MIN_FREE_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 0;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_HEADROOM_WORDS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_2m, WORDS_HEADROOM_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 2;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_WORDS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_2m, WORDS_SHARED_POOL_0_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 2;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_WORDS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_2m, WORDS_SHARED_POOL_1_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 2;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_BUFFERS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_BUFFERS_HEADROOM_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_0_BUFFERS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_BUFFERS_SHARED_POOL_0_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_SHARED_POOL_INDEX_1_BUFFERS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_BUFFERS_SHARED_POOL_1_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_OCB_HEADROOM_PDBS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_PDS_HEADROOM_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_0_PDBS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_PDS_SHARED_POOL_0_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_MIN_FREE_VSQ_SHARED_POOL_INDEX_1_PDBS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_3m, SRAM_PDS_SHARED_POOL_1_FREE_MIN_STATUSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_PACKETS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_1m, ENQ_PKT_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 1;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_ENQUEUE_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_1m, ENQ_BYTES_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 1;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_REJECT_PACKETS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_1m, RJCJ_PKT_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 1;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_REJECT_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_1m, RJCT_BYTES_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 1;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_PACKETS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_2m, DEQ_PKT_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 2;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DEQUEUE_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_2m, DEQ_BYTES_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 2;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DRAM_ENQUEUE_PACKETS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_4m, DRAM_ENQ_PKT_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DRAM_ENQUEUE_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_4m, DRAM_ENQ_BYTES_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DRAM_DEQUEUE_PACKETS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_4m, DRAM_DEQ_PKT_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYNCED_COUNTERS_DRAM_DEQUEUE_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INSTRUMENTATION_MEMORY_4m, DRAM_DEQ_BYTES_CTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, CGM_INSTRUMENTATION_MEMORYm);
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                    access_params->entry_offset.formula_cb = inst_synced_counters_records_icgm_synced_counters_min_free_ocb_buffers_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INST_SYNCED_COUNTERS_RECORDS_ICGM, table_info));
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
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_INST_TOD, is_valid, maturity_level, "INST_TOD", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOD_NTP, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TOD_1588, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TIMESTAMP_NS, DBAL_FIELD_TYPE_DEF_ARRAY8, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 48;
            db_field.permission = DBAL_PERMISSION_READONLY;
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
                access_params->access_field_id = DBAL_FIELD_TOD_NTP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_GP_STATUS_0r, TODW_NTP_TIMESTAMPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TOD_1588;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_GP_STATUS_0r, TODW_IEEE_1588_TIMECODEf));
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TIMESTAMP_NS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_GP_STATUS_1r, TODW_48_BIT_NS_TIMERf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_TIMESTAMP_NS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_GP_STATUS_1r, TODW_52_BIT_NS_TIMERf));
                        {
                            access_params->data_offset.formula_int = 4;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
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
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_clk_time_ratio_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_trigger_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_setting_icgm_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_last_count_icgn_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_synced_counters_records_icgm_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_inst_tod_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
