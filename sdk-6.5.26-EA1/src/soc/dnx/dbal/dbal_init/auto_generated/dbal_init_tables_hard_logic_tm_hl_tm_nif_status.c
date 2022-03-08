
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_rx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_RX_FIFO_STATUS;
    int is_valid = !dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_shared_arch);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_RX_FIFO_STATUS, is_valid, "NIF_RX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_J2X_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_NIF;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETHU, DBAL_FIELD_TYPE_DEF_ETHU_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RMC, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_RX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_ETHU, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, dnx_data_nif.eth.cdu_nof_get(unit)-1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLU_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_ETHU, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_2_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, dnx_data_nif.eth.cdu_nof_get(unit)-1, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLU_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_3_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDB_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDBM_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDB_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDBM_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_RX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_rx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = nif_ethu_rx_rmc_ctrl_low_prio_weight_blockindex_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_ETHU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_RX_MLF_LEVELr, RX_MLF_LEVEL_MLF_Nf));
                        access_params->array_offset.formula_cb = example_table_register_group_threshold_groupoffset_0_cb;
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_RX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ofr_rx_fifo_status_counter_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP;
    int is_valid = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP, is_valid, "NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.dbal.nof_rx_fifo_counters_get(unit));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_PORT_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit));
            db_field.max_value = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_NIF_PORT_ID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFR_RX_PORT_LEVEL_OCCUPANCY_DEBUGr, PORT_SELECTOR_FOR_LEVEL_OCCUPANCY_Nf));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ofr_rx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS;
    int is_valid = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS, is_valid, "NIF_OFR_RX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.dbal.nof_rx_fifo_counters_get(unit));
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PRIORITY_GROUP, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 16;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_PRIORITY_GROUP, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFR_RX_PORT_LEVEL_OCCUPANCY_DEBUGr, RX_PORT_N_DEBUG_RMC_FIFO_LEVEL_MAX_OCCUPANCY_FROML_RMC_384_TILL_767f));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_PRIORITY_GROUP, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFR_RX_PORT_LEVEL_OCCUPANCY_DEBUGr, RX_PORT_N_DEBUG_RMC_FIFO_LEVEL_MAX_OCCUPANCY_TILL_RMC_383f));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_PRIORITY_GROUP, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFR_RX_PORT_LEVEL_OCCUPANCY_DEBUGr, RX_PORT_N_DEBUG_RMC_FIFO_LEVEL_STATUS_FROML_RMC_384_TILL_767f));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_PRIORITY_GROUP, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFR_RX_PORT_LEVEL_OCCUPANCY_DEBUGr, RX_PORT_N_DEBUG_RMC_FIFO_LEVEL_STATUS_TILL_RMC_383f));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_cdu_tx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CDU_TX_FIFO_STATUS;
    int is_valid = !dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CDU_TX_FIFO_STATUS, is_valid, "NIF_CDU_TX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_J2X_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_NIF;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CDU, DBAL_FIELD_TYPE_DEF_CDU_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERNAL_LANE, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_nif.dbal.cdu_lane_fifo_level_and_occupancy_entry_size_get(unit);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_nif.dbal.cdu_lane_fifo_level_and_occupancy_entry_size_get(unit);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_CREDITS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_nif.dbal.tx_credits_bits_get(unit);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CDU_TX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_CDU, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_2_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_CDU, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_2_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_CDU, dnx_data_nif.eth.cdu_nof_get(unit), (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_2_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDB_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDBM_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDB_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDBM_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDB_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDBM_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDU_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = nif_cdu_tx_lane_ctrl_start_thr_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                    }
                }
                {
                    int condition_index = 0;
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_CDU, 0, (uint32) DBAL_DB_INVALID));
                    condition_index++;
                    access_params->nof_conditions = condition_index;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CDUM_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->block_index.formula_cb = example_table_register_group_threshold_blockindex_1_cb;
                        access_params->data_offset.formula_cb = nif_cdu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CDU_TX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_clu_tx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_CLU_TX_FIFO_STATUS;
    int is_valid = dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_is_clu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_CLU_TX_FIFO_STATUS, is_valid, "NIF_CLU_TX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_J2X_NOTREV;
    table_entry->table_labels[1] = DBAL_LABEL_NIF;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLU, DBAL_FIELD_TYPE_DEF_CLU_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERNAL_LANE, DBAL_FIELD_TYPE_DEF_NIF_INTERNAL_LANE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_nif.dbal.clu_lane_fifo_level_and_occupancy_entry_size_get(unit);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_nif.dbal.clu_lane_fifo_level_and_occupancy_entry_size_get(unit);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PM_CREDITS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_CLU_TX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLU_TX_MLF_MAX_OCCUPANCYr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->block_index.formula_cb = nif_clu_tx_lane_ctrl_start_thr_blockindex_0_cb;
                    access_params->data_offset.formula_cb = nif_clu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLU_TX_MLF_LEVEL_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->block_index.formula_cb = nif_clu_tx_lane_ctrl_start_thr_blockindex_0_cb;
                    access_params->data_offset.formula_cb = nif_clu_tx_fifo_status_max_occupancy_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PM_CREDITS;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CLU_TX_MLF_NUM_CREDITS_FROM_PMr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->block_index.formula_cb = nif_clu_tx_lane_ctrl_start_thr_blockindex_0_cb;
                    access_params->data_offset.formula_cb = nif_clu_tx_fifo_status_pm_credits_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_CLU_TX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ilu_hrf_rx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILU_HRF_RX_FIFO_STATUS;
    int is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_ilu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILU_HRF_RX_FIFO_STATUS, is_valid, "NIF_ILU_HRF_RX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 3);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ILKN_CORE_ID, DBAL_FIELD_TYPE_DEF_ILKN_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ILKN_PORT_ID, DBAL_FIELD_TYPE_DEF_ILKN_PORT_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ILKN_HRF_TYPE, DBAL_FIELD_TYPE_DEF_ILKN_HRF_TYPE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 13;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 13;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILU_HRF_RX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ILU_HRF_RX_CONTROLLER_MAX_OCCUPANCYr, HRF_RX_MAX_OCCUPANCY_HRF_Nf));
                    access_params->array_offset.formula_cb = nif_ilu_rx_hrf_flush_flush_hrf_dataoffset_0_cb;
                    access_params->block_index.formula_cb = nif_ile_core_enablers_reset_blockindex_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ILU_HRF_RX_CONTROLLER_DEBUGr, HRF_RX_LEVEL_HRF_Nf));
                    access_params->array_offset.formula_cb = nif_ilu_rx_hrf_flush_flush_hrf_dataoffset_0_cb;
                    access_params->block_index.formula_cb = nif_ile_core_enablers_reset_blockindex_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILU_HRF_RX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ilu_hrf_tx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_ILU_HRF_TX_FIFO_STATUS;
    int is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_ilu_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_ILU_HRF_TX_FIFO_STATUS, is_valid, "NIF_ILU_HRF_TX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ILKN_CORE_ID, DBAL_FIELD_TYPE_DEF_ILKN_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ILKN_PORT_ID, DBAL_FIELD_TYPE_DEF_ILKN_PORT_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_ILU_HRF_TX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ILU_HRF_TX_CONTROLLER_DEBUGr, HRF_TX_MAX_OCCUPANCY_HRF_Nf));
                    access_params->array_offset.formula_cb = nif_ile_status_is_latch_low_aligned_arrayoffset_0_cb;
                    access_params->block_index.formula_cb = nif_ile_core_enablers_reset_blockindex_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ILU_HRF_TX_CONTROLLER_DEBUGr, HRF_TX_LEVEL_HRF_Nf));
                    access_params->array_offset.formula_cb = nif_ile_status_is_latch_low_aligned_arrayoffset_0_cb;
                    access_params->block_index.formula_cb = nif_ile_core_enablers_reset_blockindex_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_ILU_HRF_TX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_oft_tx_fifo_status_counter_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP;
    int is_valid = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP, is_valid, "NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.dbal.nof_tx_fifo_counters_get(unit));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NIF_PORT_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit));
            db_field.max_value = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_NIF_PORT_ID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFT_TX_PORTS_TO_DEBUGr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = nif_oft_tx_fifo_status_counter_map_nif_port_id_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_oft_tx_fifo_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS;
    int is_valid = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS, is_valid, "NIF_OFT_TX_FIFO_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_NIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_nif.dbal.nof_tx_fifo_counters_get(unit));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_OCCUPANCY, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_LEVEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_OCCUPANCY;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFT_TX_PORT_DEBUGr, TX_PORT_N_DEBUG_TMC_FIFO_LEVEL_MAX_OCCUPANCYf));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_LEVEL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OFT_TX_PORT_DEBUGr, TX_PORT_N_DEBUG_TMC_FIFO_LEVEL_STATUSf));
                    access_params->array_offset.formula_cb = nif_ofr_rx_fifo_status_counter_map_nif_port_id_arrayoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_nif_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_rx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ofr_rx_fifo_status_counter_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ofr_rx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_cdu_tx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_clu_tx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ilu_hrf_rx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_ilu_hrf_tx_fifo_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_oft_tx_fifo_status_counter_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_nif_status_nif_oft_tx_fifo_status_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
