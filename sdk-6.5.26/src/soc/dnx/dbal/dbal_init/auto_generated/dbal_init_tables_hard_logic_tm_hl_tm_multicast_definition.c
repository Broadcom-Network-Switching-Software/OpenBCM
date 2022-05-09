
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_mcdb_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MCDB;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MCDB, is_valid, "MCDB", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MCDB_INDEX, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit);
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 9);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
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
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "SINGLE_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT;
        field_index = 0;
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
            DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 5;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 18;
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
        }
        else
        {
            table_entry->multi_res_info[result_type_counter].is_disabled = TRUE;
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0);
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "SINGLE_EXT_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
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
            db_field.size = 9;
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "DOUBLE_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BMP_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BITMAP_PTR_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LINK_LIST_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "LINK_LIST_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BMP_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BMP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BITMAP_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT;
        field_index = 0;
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
            DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 5;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 18;
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
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BMP_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 5;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BMP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 32;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
        }
        else
        {
            table_entry->multi_res_info[result_type_counter].is_disabled = TRUE;
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0);
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BITMAP_EXT_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_TDM_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2 + 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            db_field.nof_instances = 4;
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
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "TDM_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT;
        field_index = 0;
        if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
        {
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2 + 3);
            DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.size = 5;
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
                field_index += dbal_db_field->nof_instances;
            }
            {
                dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
                SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_DESTINATION_EXT, DBAL_FIELD_TYPE_DEF_EGRESS_DESTINATION_EXT, dbal_db_field));
                dbal_db_init_table_field_params_init(&db_field);
                db_field.nof_instances = 3;
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
        }
        else
        {
            table_entry->multi_res_info[result_type_counter].is_disabled = TRUE;
            DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0);
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "TDM_EXT_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MCDB, is_standard_1, is_compatible_with_all_images));
    }
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT], 0)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT], 1)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT], 2)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT], 3)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT], 24)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT], 25)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT], 26)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT], 27)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT], 8)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT], 9)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT], 10)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT], 11)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT], 4)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT], 12)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT], 5)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT], 13)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT], 21)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT], 29)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT], 6)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT], 14)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_TDM_FORMAT], 7)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_TDM_FORMAT], 15)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT], 23)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT], 31)
    {
        int map_idx = 0;
        int access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "SINGLE_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_SINGLEm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_SINGLEm, FORMATf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 1;
                access_params->access_offset = 3;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_SINGLEm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_SINGLEm, CUDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_SINGLEm, DESTINATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "SINGLE_EXT_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
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
                    access_params->access_size = 1;
                    access_params->access_offset = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_SW;
                    access_params->access_size = 1;
                    access_params->access_offset = 3;
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, NEXT_PTRf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, CUDf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, TCMf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_EGR_SINGLEm, DESTINATIONf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
        map_idx = DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "DOUBLE_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, FORMATf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 1;
                access_params->access_offset = 3;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, CUDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, TCMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, DESTINATION_Af));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_DOUBLEm, DESTINATION_Bf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BITMAP_PTR_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, FORMAT_LSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, FORMAT_MSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, CUDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BMP_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, BITMAP_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "LINK_LIST_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_LINK_LISTm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_LINK_LISTm, FORMAT_LSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_LINK_LISTm, FORMAT_MSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_LINK_LISTm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LINK_LIST_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_LINK_LISTm, PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BITMAP_EXT_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
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
                        access_params->access_size = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, FORMAT_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, FORMAT_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 4;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, NEXT_PTRf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_BMP_OFFSET;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, OFFSETf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_BMP;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_BITMAPm, BITMAPf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_size = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, FORMAT_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, FORMAT_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 4;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, NEXT_PTRf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, TCMf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_BMP_OFFSET;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, OFFSETf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_BMP;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_BITMAPm, BITMAPf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
        map_idx = DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BITMAP_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, FORMAT_LSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, FORMAT_MSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, TCMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BMP_OFFSET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, OFFSETf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BMP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAPm, BITMAPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "TDM_EXT_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
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
                        access_params->access_size = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, FORMAT_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, FORMAT_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 4;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, PP_DSP_Af));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = 1;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, PP_DSP_Bf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 7;
                        access_params->inst_idx = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, PP_DSP_C_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 4;
                        access_params->access_offset = 7;
                        access_params->inst_idx = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, PP_DSP_C_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_field_id = DBAL_FIELD_CUD;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2_C_TDMm, CUDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                        access_params->access_size = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, FORMAT_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, FORMAT_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 1;
                        access_params->access_offset = 4;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, J_2C_FORMATf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, PP_DSP_Af));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = 1;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, PP_DSP_Bf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 7;
                        access_params->inst_idx = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, PP_DSP_C_LSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION_EXT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->access_size = 4;
                        access_params->access_offset = 7;
                        access_params->inst_idx = 2;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, PP_DSP_C_MSBf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_J_2C_TDMm, CUDf));
                            DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                            access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                            access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                            access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
        map_idx = DBAL_RESULT_TYPE_MCDB_TDM_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "TDM_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    access_params->access_size = 1;
                    access_params->access_offset = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, J_2C_FORMATf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                        access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                        access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, FORMAT_LSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, FORMAT_MSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, PP_DSP_Af));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, PP_DSP_Bf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->inst_idx = 2;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, PP_DSP_Cf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->inst_idx = 3;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, PP_DSP_Df));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_TDMm, CUDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MCDB, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_mcdb_link_list_ptr_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MCDB_LINK_LIST_PTR;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MCDB_LINK_LIST_PTR, is_valid, "MCDB_LINK_LIST_PTR", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MCDB_INDEX, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NEXT_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PREV_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 18;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MCDB_LINK_LIST_PTR, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_NEXT_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_MCDB_FORMAT_BITMAP_PTRm, NEXT_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, MTM_MCDBm);
                    access_params->array_offset.formula_cb = mcdb_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = mcdb_result_type_entryoffset_0_cb;
                    access_params->alias_data_offset.formula_cb = example_for_hl_with_memory_aliasing_mpls_out_aliasmemoryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PREV_PTR;
                access_params->access_type = DBAL_HL_ACCESS_SW;
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MCDB_LINK_LIST_PTR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_group_status_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MULTICAST_GROUP_STATUS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MULTICAST_GROUP_STATUS, is_valid, "MULTICAST_GROUP_STATUS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_ID, DBAL_FIELD_TYPE_DEF_GROUP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit);
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_GROUP_IS_OPEN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_GROUP_IS_OPEN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INGRESS_BITMAP_IS_OPEN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EGRESS_BITMAP_IS_OPEN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MULTICAST_GROUP_STATUS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_INGRESS_GROUP_IS_OPEN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_IS_ING_MCm, IS_ING_MC_BITMAPf));
                    access_params->entry_offset.formula_cb = egress_same_interface_filter_same_interface_filter_enable_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = egress_same_interface_filter_same_interface_filter_enable_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_GROUP_IS_OPEN;
                access_params->access_type = DBAL_HL_ACCESS_SW;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INGRESS_BITMAP_IS_OPEN;
                access_params->access_type = DBAL_HL_ACCESS_SW;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EGRESS_BITMAP_IS_OPEN;
                access_params->access_type = DBAL_HL_ACCESS_SW;
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MULTICAST_GROUP_STATUS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_offsets_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MULTICAST_OFFSETS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MULTICAST_OFFSETS, is_valid, "MULTICAST_OFFSETS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2 + (((dnx_data_device.general.nof_cores_get(unit)/2)>0)?1:0));
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
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MULTICAST_OFFSETS, is_standard_1, is_compatible_with_all_images));
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_OFFSETr, ING_0_MCDB_OFFSETf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_INGRESS_OFFSET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_OFFSETr, ING_1_MCDB_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_ING_MCDB_OFFSETr, ING_MCDB_OFFSETf));
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_OFFSETr, EGR_0_MCDB_OFFSETf));
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2X_A0)
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_OFFSETr, EGR_MCDB_OFFSETf));
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
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, MTM_EGR_MCDB_OFFSETr, EGR_1_MCDB_OFFSETf));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MULTICAST_OFFSETS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_ingress_internal_stat_count_copies_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES, is_valid, "MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COMMAND_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            db_field.max_value = 9;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES, is_standard_1, is_compatible_with_all_images));
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
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_0f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_1f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 2, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_2f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 3, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_3f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 4, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_4f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 5, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_5f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 6, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_6f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 7, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_7f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 8, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_8f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_COMMAND_ID, 9, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_INT_IRPP_STAT_SETTINGSr, INT_IRPP_STAT_MC_ONCE_9f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_ingress_external_stat_count_copies_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES, is_valid, "MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ID, DBAL_FIELD_TYPE_DEF_CORE_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_EXT_STAT_BILL_SETTINGSr, EXT_STAT_BILL_MC_ONCEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_group_egr_cores_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MULTICAST_GROUP_EGR_CORES;
    int is_valid = dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_MULTICAST_GROUP_EGR_CORES, is_valid, "MULTICAST_GROUP_EGR_CORES", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_MULTICAST;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 2);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_ID, DBAL_FIELD_TYPE_DEF_GROUP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_REP_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CORE_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_MULTICAST_GROUP_EGR_CORES, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_CORE_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, FDR_FDR_MCm, PAYLOADf));
                    access_params->entry_offset.formula_cb = multicast_group_egr_cores_core_enable_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = multicast_group_egr_cores_core_enable_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MULTICAST_GROUP_EGR_CORES, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_mcdb_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_mcdb_link_list_ptr_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_group_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_offsets_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_ingress_internal_stat_count_copies_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_ingress_external_stat_count_copies_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_multicast_group_egr_cores_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
