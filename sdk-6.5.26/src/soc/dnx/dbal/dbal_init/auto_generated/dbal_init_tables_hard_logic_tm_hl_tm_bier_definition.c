
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_ing_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BIER_ING_TABLE;
    int is_valid = dnx_data_bier.params.feature_get(unit, dnx_data_bier_params_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BIER_ING_TABLE, is_valid, "BIER_ING_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    table_entry->table_labels[1] = DBAL_LABEL_BIER;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFR_ID, DBAL_FIELD_TYPE_DEF_BFR_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BASE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            db_field.max_value = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BITSTR, DBAL_FIELD_TYPE_DEF_BITMAP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 256;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CUD, DBAL_FIELD_TYPE_DEF_CUD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_TYPE_DEF_DESTINATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BIER_ING_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_BITSTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDBm, DATAf));
                    access_params->array_offset.formula_cb = bier_ing_table_bitstr_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_bitstr_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CUD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_ING_INFOm, EEDB_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_ING_INFOm, DESTINATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_ING_INFOm, VALIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BIER_ING_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_egr_0_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BIER_EGR_0_TABLE;
    int is_valid = dnx_data_bier.params.feature_get(unit, dnx_data_bier_params_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BIER_EGR_0_TABLE, is_valid, "BIER_EGR_0_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    table_entry->table_labels[1] = DBAL_LABEL_BIER;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFR_ID, DBAL_FIELD_TYPE_DEF_BFR_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BASE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            db_field.max_value = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 2);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        result_type_counter = DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BITSTR, DBAL_FIELD_TYPE_DEF_BITMAP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 256;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CUD, DBAL_FIELD_TYPE_DEF_CUD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCM, DBAL_FIELD_TYPE_DEF_TCM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "NORMAL_DESTINATION", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION;
        field_index = 0;
        if (DBAL_IS_J2C_A0)
        {
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
            DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 1;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BITSTR, DBAL_FIELD_TYPE_DEF_BITMAP, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 256;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CUD, DBAL_FIELD_TYPE_DEF_CUD, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCM, DBAL_FIELD_TYPE_DEF_TCM, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION_EXT, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION_EXT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
        }
        else
        {
            table_entry->multi_res_info[result_type_counter].is_disabled = TRUE;
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0);
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "EXTENDED_DESTINATION", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BIER_EGR_0_TABLE, is_standard_1, is_compatible_with_all_images));
    }
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION], 0)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION], 1)
    {
        int map_idx = 0;
        int access_counter;
        map_idx = DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "NORMAL_DESTINATION", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_SW;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BITSTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDBm, DATAf));
                    access_params->array_offset.formula_cb = bier_ing_table_bitstr_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_bitstr_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CUD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, EEDB_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCM;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, TCMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, DESTINATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, VALIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "EXTENDED_DESTINATION", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0)
        {
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_BITSTR;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDBm, DATAf));
                        access_params->array_offset.formula_cb = bier_ing_table_bitstr_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = bier_ing_table_bitstr_entryoffset_0_cb;
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_CUD;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, EEDB_POINTERf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_TCM;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, DESTINATIONf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_VALID;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, VALIDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_CUD;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, EEDB_POINTERf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_TCM;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, DESTINATIONf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_VALID;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, VALIDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BIER_EGR_0_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_egr_1_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BIER_EGR_1_TABLE;
    int is_valid = dnx_data_bier.params.feature_get(unit, dnx_data_bier_params_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BIER_EGR_1_TABLE, is_valid, "BIER_EGR_1_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    table_entry->table_labels[1] = DBAL_LABEL_BIER;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFR_ID, DBAL_FIELD_TYPE_DEF_BFR_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BASE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            db_field.max_value = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 2);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        result_type_counter = DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_NORMAL_DESTINATION;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BITSTR, DBAL_FIELD_TYPE_DEF_BITMAP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 256;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CUD, DBAL_FIELD_TYPE_DEF_CUD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCM, DBAL_FIELD_TYPE_DEF_TCM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "NORMAL_DESTINATION", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_EXTENDED_DESTINATION;
        field_index = 0;
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
            DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 1;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BITSTR, DBAL_FIELD_TYPE_DEF_BITMAP, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 256;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CUD, DBAL_FIELD_TYPE_DEF_CUD, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TCM, DBAL_FIELD_TYPE_DEF_TCM, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION_EXT, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION_EXT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
        }
        else
        {
            table_entry->multi_res_info[result_type_counter].is_disabled = TRUE;
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0);
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "EXTENDED_DESTINATION", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BIER_EGR_1_TABLE, is_standard_1, is_compatible_with_all_images));
    }
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_NORMAL_DESTINATION], 0)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_EXTENDED_DESTINATION], 1)
    {
        int map_idx = 0;
        int access_counter;
        map_idx = DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_NORMAL_DESTINATION;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "NORMAL_DESTINATION", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_SW;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BITSTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDBm, DATAf));
                    access_params->array_offset.formula_cb = bier_ing_table_bitstr_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_bitstr_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CUD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, EEDB_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TCM;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, TCMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, DESTINATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BIER_EGR_INFOm, VALIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_BIER_EGR_1_TABLE_EXTENDED_DESTINATION;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "EXTENDED_DESTINATION", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_BITSTR;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDBm, DATAf));
                        access_params->array_offset.formula_cb = bier_ing_table_bitstr_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = bier_ing_table_bitstr_entryoffset_0_cb;
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_CUD;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, EEDB_POINTERf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_TCM;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, DESTINATIONf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            if (DBAL_IS_J2C_A0)
            {
                {
                    table_db_access_params_struct_t * access_params;
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_VALID;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BIER_EGR_INFOm, VALIDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_CUD;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, EEDB_POINTERf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_TCM;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, DESTINATIONf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_VALID;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BIER_EGR_INFOm, VALIDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = bier_ing_table_cud_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = bier_ing_table_cud_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = bier_ing_table_cud_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BIER_EGR_1_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_offsets_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_BIER_OFFSETS;
    int is_valid = dnx_data_bier.params.feature_get(unit, dnx_data_bier_params_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_BIER_OFFSETS, is_valid, "BIER_OFFSETS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    table_entry->table_labels[1] = DBAL_LABEL_BIER;
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3 + (((dnx_data_device.general.nof_cores_get(unit)/2)>0)?1:0) + (((dnx_data_device.general.nof_cores_get(unit)/2)>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 19;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_CORE_0_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 19;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_device.general.nof_cores_get(unit)/2 != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_CORE_1_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_device.general.nof_cores_get(unit)/2;
            db_field.size = 19;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETPP_CORE_0_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 19;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_device.general.nof_cores_get(unit)/2 != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ETPP_CORE_1_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_device.general.nof_cores_get(unit)/2;
            db_field.size = 19;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_BIER_OFFSETS, is_standard_1, is_compatible_with_all_images));
    }
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INGRESS_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_BIER_OFFSETr, ING_0_MCDB_BIER_OFFSETf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INGRESS_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_BIER_OFFSETr, ING_1_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_EGRESS_CORE_0_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_BIER_OFFSETr, EGR_0_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_EGRESS_CORE_1_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_BIER_OFFSETr, EGR_1_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ETPP_CORE_0_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EQP_MCDB_BIER_OFFSETr, EQP_0_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ETPP_CORE_1_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EQP_MCDB_BIER_OFFSETr, EQP_1_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INGRESS_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_BIER_OFFSETr, ING_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_EGRESS_CORE_0_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_BIER_OFFSETr, EGR_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ETPP_CORE_0_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EQP_MCDB_BIER_OFFSETr, EQP_MCDB_BIER_OFFSETf));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_BIER_OFFSETS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_ing_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_egr_0_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_egr_1_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_bier_offsets_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
