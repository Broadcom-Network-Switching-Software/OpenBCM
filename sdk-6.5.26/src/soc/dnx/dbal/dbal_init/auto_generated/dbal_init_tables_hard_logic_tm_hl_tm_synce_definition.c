
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_enable_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_ENABLE_CTRL;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_ENABLE_CTRL, is_valid, "SYNCE_ENABLE_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESET, DBAL_FIELD_TYPE_DEF_NIF_RESET_REVERSE_LOGIC, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_ENABLE_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNC_E_RSTNr, NIF_SYNC_E_RSTNf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
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
                    access_params->access_field_id = DBAL_FIELD_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CORE_ID, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNC_E_RSTNr, NIF_CORE_0_SYNC_E_RSTNf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CORE_ID, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNC_E_RSTNr, NIF_CORE_1_SYNC_E_RSTNf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_ENABLE_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_nif_fab_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_NIF_FAB_CTRL;
    int is_valid = (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist)+dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))*(!dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync));
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, is_valid, "SYNCE_NIF_FAB_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + (((dnx_data_fabric.links.nof_links_get(unit))>0)?1:0) + (((dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))>0)?1:0) + (((dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        if (dnx_data_fabric.links.nof_links_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_FAB_LINK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_fabric.links.nof_links_get(unit);
            db_field.size = utilex_log2_round_up(dnx_data_fabric.links.nof_links_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_NIF_CORE_SEL, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_NIF_FAB_SEL, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_FAB_LINK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_CFGr, MASTER_CFG_FAB_SELf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_FAB_LINK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_CFGr, SLAVE_CFG_FAB_SELf));
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_CORE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_CFGr, MASTER_CFG_NIF_SELf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_CORE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_CFGr, SLAVE_CFG_NIF_SELf));
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_FAB_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_CFGr, MASTER_CFG_NIF_FAB_SELf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_FAB_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_CFGr, SLAVE_CFG_NIF_FAB_SELf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0)
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_FAB_LINK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_PLL_CONFIGr, SYNCE_MASTER_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 276;
                        }
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_FAB_LINK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_PLL_CONFIGr, SYNCE_SLAVE_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 276;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_CORE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_PLL_CONFIGr, SYNCE_MASTER_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 283;
                        }
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_CORE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_PLL_CONFIGr, SYNCE_SLAVE_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 283;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_FAB_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_MASTER_PLL_CONFIGr, SYNCE_MASTER_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 284;
                        }
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_SYNCE_NIF_FAB_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNCE_SLAVE_PLL_CONFIGr, SYNCE_SLAVE_PLL_CTRLf));
                        {
                            access_params->data_offset.formula_int = 284;
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_NIF_FAB_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_output_clk_mode_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_OUTPUT_CLK_MODE_CTRL;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_OUTPUT_CLK_MODE_CTRL, is_valid, "SYNCE_OUTPUT_CLK_MODE_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLK0_OUTPUT_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLK1_OUTPUT_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_OUTPUT_CLK_MODE_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_CLK0_OUTPUT_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNC_ETH_PAD_0_OE_Nf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLK1_OUTPUT_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNC_ETH_PAD_1_OE_Nf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_CLK0_OUTPUT_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNC_ETH_PAD_0_OE_Nf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLK1_OUTPUT_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNC_ETH_PAD_1_OE_Nf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_OUTPUT_CLK_MODE_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_output_clk_force_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_OUTPUT_CLK_FORCE_CTRL;
    int is_valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_ouput_force_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_OUTPUT_CLK_FORCE_CTRL, is_valid, "SYNCE_OUTPUT_CLK_FORCE_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLK0_OUTPUT_FORCE_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLK1_OUTPUT_FORCE_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_OUTPUT_CLK_FORCE_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_CLK0_OUTPUT_FORCE_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNC_ETH_ENABLE_FORCEr, SYNC_ETH_0_ENABLE_FORCEf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLK1_OUTPUT_FORCE_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CFC_SYNC_ETH_ENABLE_FORCEr, SYNC_ETH_1_ENABLE_FORCEf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_OUTPUT_CLK_FORCE_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_counter_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_COUNTER;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_COUNTER, is_valid, "SYNCE_COUNTER", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 3);
        if (!dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync) != 0)
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync) != 0)
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync) != 0)
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)-1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + (((dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_COUNTER, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2X_A0)
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
                    access_params->access_field_id = DBAL_FIELD_COUNTER;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_0f));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_COUNTER1;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_1f));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_COUNTER;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_COUNTERr, SYNC_ETH_COUNTER_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_COUNTER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_clk_counter_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_CLK_COUNTER_CTRL;
    int is_valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_CLK_COUNTER_CTRL, is_valid, "SYNCE_CLK_COUNTER_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)-1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CAPTURE_MODE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER1_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_CLK_COUNTER_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2X_A0)
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
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_ENABLEf));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CAPTURE_MODE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_CAPTURE_MODEf));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_COUNTER_CLK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_0_CLK_SELECTIONf));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_COUNTER1_CLK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_DEBUG_COUNTERr, SYNCE_DEBUG_COUNTER_1_CLK_SELECTIONf));
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_CLK_COUNTER_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_chain_enable_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, is_valid, "SYNCE_REF_CLK_NIF_CHAIN_ENABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit)-1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit)-1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, dnx_data_nif.eth.cdu_nof_get(unit)-1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CHAIN_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_REF_CLK_NIF_CHAIN_ENABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_cdu_lane_sel_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL, is_valid, "SYNCE_REF_CLK_NIF_CDU_LANE_SEL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLOCK_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LINK_STATUS_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
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
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
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
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_1_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->access_size = 3;
                    access_params->access_offset = 0;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_NIF_SYNCE_CLK_BLK, 6, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_3_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_REF_CLK_NIF_CDU_LANE_SEL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_clu_lane_sel_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL, is_valid, "SYNCE_REF_CLK_NIF_CLU_LANE_SEL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.min_value = dnx_data_nif.eth.cdu_nof_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLOCK_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LINK_STATUS_LANE_SEL, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_CLOCK_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_VALID_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_VALID_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_STATUS_LANE_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLUP_SYNC_ETH_CFGr, SYNC_ETH_LINK_STATUS_SEL_Nf));
                        access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_ref_clk_nif_chain_enable_enable_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_REF_CLK_NIF_CLU_LANE_SEL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_div_nif_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, is_valid, "SYNCE_REF_CLK_DIV_NIF_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLOCK_DIV_MODE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DIV_PHASE_ZERO, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DIV_PHASE_ONE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_CLOCK_DIV_MODE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_CLOCK_DIV_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DIV_PHASE_ZERO;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_DIVIDER_PHASE_ZERO_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DIV_PHASE_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_DIVIDER_PHASE_ONE_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_REF_CLK_DIV_NIF_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_core_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_CORE_CTRL;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_CORE_CTRL, is_valid, "SYNCE_CORE_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3 + (((dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_nmg_chain_select))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SQUELCH_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LINK_VALID_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GTIMER_MODE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_nmg_chain_select) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NMG_CHAIN_SEL, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_nmg_chain_select);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_CORE_CTRL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_SQUELCH_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_SQUELCH_EN_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LINK_VALID_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_LINK_VALID_SEL_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GTIMER_MODE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CFGr, SYNC_ETH_GTIMER_MODE_Nf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NMG_CHAIN_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CHAIN_SEL_FOR_CLOCK_VLDr, SYNC_ETH_CHAIN_SEL_N_FOR_CLOCK_VLDf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NMG_CHAIN_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CHAIN_SEL_FOR_LINK_STATUSr, SYNC_ETH_CHAIN_SEL_N_FOR_LINK_STATUSf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NMG_CHAIN_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_CHAIN_SEL_FOR_SYNCE_CLK_WINNERr, SYNC_ETH_CHAIN_SEL_N_FOR_SYNCE_CLK_WINNERf));
                    access_params->array_offset.formula_cb = synce_counter_counter_arrayoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_CORE_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_auto_sequelch_th_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_AUTO_SEQUELCH_TH_CTRL;
    int is_valid = !dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_AUTO_SEQUELCH_TH_CTRL, is_valid, "SYNCE_AUTO_SEQUELCH_TH_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SQUELCH_EN_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SQUELCH_DIS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_AUTO_SEQUELCH_TH_CTRL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_SQUELCH_EN_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_SQUELCH_EN_TH_REGr, SYNC_ETH_SQUELCH_EN_THf));
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SQUELCH_DIS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, NMG_SYNC_ETH_SQUELCH_DIS_TH_REGr, SYNC_ETH_SQUELCH_DIS_THf));
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_AUTO_SEQUELCH_TH_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_nif_cdu_clk_valid_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_NIF_CDU_CLK_VALID_CTRL;
    int is_valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_NIF_CDU_CLK_VALID_CTRL, is_valid, "SYNCE_NIF_CDU_CLK_VALID_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_NIF_SYNCE_CLK_BLK_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.cdu_nof_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LANE_ID, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit)-1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LINK_VALID_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_NIF_CDU_CLK_VALID_CTRL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2X_A0)
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
                    access_params->access_field_id = DBAL_FIELD_ENABLE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_VALID_CTRLr, SYNCE_RX_CLK_GATE_EN_Nf));
                        access_params->array_offset.formula_cb = synce_nif_cdu_clk_valid_ctrl_enable_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LINK_VALID_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDPM_SYNCE_CLK_VALID_CTRLr, SYNCE_RX_VAILD_CTRL_Nf));
                        access_params->array_offset.formula_cb = synce_nif_cdu_clk_valid_ctrl_enable_arrayoffset_0_cb;
                        access_params->block_index.formula_cb = synce_counter_counter_blockindex_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_NIF_CDU_CLK_VALID_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_fab_lane_sel_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_SYNCE_REF_CLK_NIF_FAB_LANE_SEL;
    int is_valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_SYNCE_REF_CLK_NIF_FAB_LANE_SEL, is_valid, "SYNCE_REF_CLK_NIF_FAB_LANE_SEL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYNCE_INDEX, DBAL_FIELD_TYPE_DEF_SYNCE_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_FAB_VALID_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_FAB_CLOCK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_SYNCE_REF_CLK_NIF_FAB_LANE_SEL, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2X_A0)
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
                    access_params->access_field_id = DBAL_FIELD_NIF_FAB_VALID_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNCE_CLK_CTRLr, SYNCE_MASTER_CLK_VALID_SELf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NIF_FAB_VALID_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNCE_CLK_CTRLr, SYNCE_SLAVE_CLK_VALID_SELf));
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
                    access_params->access_field_id = DBAL_FIELD_NIF_FAB_CLOCK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNCE_CLK_CTRLr, SYNCE_MASTER_CLK_SELf));
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NIF_FAB_CLOCK_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SYNCE_INDEX, 1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SYNCE_CLK_CTRLr, SYNCE_SLAVE_CLK_SELf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_SYNCE_REF_CLK_NIF_FAB_LANE_SEL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fdr_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FDR_CTRL;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FDR_CTRL, is_valid, "FABRIC_SYNCE_REF_CLK_FDR_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MASTER_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_fabric.links.nof_links_per_core_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SLAVE_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_fabric.links.nof_links_per_core_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FDR_CTRL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_MASTER_CLK_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FDR_SYNC_E_SELECTr, MASTER_CLK_SELf));
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SLAVE_CLK_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FDR_SYNC_E_SELECTr, SLAVE_CLK_SELf));
                    access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FDR_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fmac_sel_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FMAC_SEL;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist)*(!dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_in_nanosync));
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FMAC_SEL, is_valid, "FABRIC_SYNCE_REF_CLK_FMAC_SEL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAS_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.max_value = dnx_data_synce.general.nof_mas_get(unit)-1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MASTER_FMAC_INSTANCE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_fabric.blocks.nof_instances_fsrd_get(unit)/dnx_data_synce.general.nof_mas_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SLAVE_FMAC_INSTANCE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_fabric.blocks.nof_instances_fsrd_get(unit)/dnx_data_synce.general.nof_mas_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FMAC_SEL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_MASTER_FMAC_INSTANCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_MAS_ID, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNCE_MASTER_FMAC_0_SELf));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MASTER_FMAC_INSTANCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_MAS_ID, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNCE_MASTER_FMAC_1_SELf));
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
                access_params->access_field_id = DBAL_FIELD_SLAVE_FMAC_INSTANCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_MAS_ID, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNCE_SLAVE_FMAC_0_SELf));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SLAVE_FMAC_INSTANCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_MAS_ID, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, SYNCE_SLAVE_FMAC_1_SELf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FMAC_SEL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fsrd_ctrl_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL;
    int is_valid = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, is_valid, "FABRIC_SYNCE_REF_CLK_FSRD_CTRL", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_SYNCE;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FABRIC_SYNCE_CLK_BLK, DBAL_FIELD_TYPE_DEF_FABRIC_SYNCE_CLK_BLK_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2 + (((dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))>0)?1:0) + (((dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MASTER_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SLAVE_CLK_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DIV_PHASE_0, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DIV_PHASE_1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_synce.general.feature_get(unit, dnx_data_synce_general_synce_no_sdm_mode);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, is_standard_1, is_compatible_with_all_images));
    }
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
                access_params->access_field_id = DBAL_FIELD_MASTER_CLK_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FSRD_SYNC_E_SELECTr, MASTER_CLK_SELf));
                    access_params->block_index.formula_cb = fabric_synce_ref_clk_fsrd_ctrl_master_clk_sel_blockindex_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SLAVE_CLK_SEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FSRD_SYNC_E_SELECTr, SLAVE_CLK_SELf));
                    access_params->block_index.formula_cb = fabric_synce_ref_clk_fsrd_ctrl_master_clk_sel_blockindex_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DIV_PHASE_0;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FSRD_SYNC_E_SELECTr, SYNC_E_CLK_DIV_PHASE_0f));
                    access_params->block_index.formula_cb = fabric_synce_ref_clk_fsrd_ctrl_master_clk_sel_blockindex_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DIV_PHASE_1;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FSRD_SYNC_E_SELECTr, SYNC_E_CLK_DIV_PHASE_1f));
                    access_params->block_index.formula_cb = fabric_synce_ref_clk_fsrd_ctrl_master_clk_sel_blockindex_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_FABRIC_SYNCE_REF_CLK_FSRD_CTRL, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_enable_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_nif_fab_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_output_clk_mode_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_output_clk_force_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_counter_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_clk_counter_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_chain_enable_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_cdu_lane_sel_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_clu_lane_sel_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_div_nif_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_core_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_auto_sequelch_th_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_nif_cdu_clk_valid_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_synce_ref_clk_nif_fab_lane_sel_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fdr_ctrl_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fmac_sel_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_fabric_synce_ref_clk_fsrd_ctrl_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
